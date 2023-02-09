#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "parser.h"
#include <stdint.h>
#include "linkedlist.h"

// ? questions:
// ! adding "cat > a &" to job list -> there's a lag before the cmd gets updated to stopped

// !1: waitpid for multi process command: sleep 3 | sleep 5 | sleep 10 &

// ! :check the status code to find out if the process is stopped or terminated or signaled (look at the peer review code snippet

int getInput(size_t *maxLineLength, char *userInput);
void commandInfo(struct parsed_command *cmd);
int handleBytesRead(int *numBytes, char *cmd);
void handleSingleCmd(struct parsed_command *cmd, char *input);
void handleMultiCmd(struct parsed_command *cmd, char *input);
void signalHandler(int signum);

int signalCaught = 0;
struct node *jobList = NULL; // head of linked list of BG process
char RUNNING[] = "running";
char STOPPED[] = "stopped";
pid_t shellPid;

int main(int argc, char *argv[])
{
    size_t maxLineLength = 4096;

    // register signal handlers
    if (signal(SIGINT, signalHandler) == SIG_ERR || signal(SIGTTOU, SIG_IGN) || signal(SIGTSTP, signalHandler) == SIG_ERR)
    {
        perror("signal failed");
        exit(EXIT_FAILURE);
    }

    shellPid = getpgrp();
    printf("Shell pgid: %d\n", shellPid);

    while (1)
    {
        printf("BG list/job queues: ");
        printList(jobList);

        struct node *pointer = jobList;

        // check for each running BG process and remove from job list if process is done
        while (pointer != NULL)
        {
            int status;
            pid_t waitPid = waitpid(pointer->pid, &status, WNOHANG | WUNTRACED);
            // if (waitPid != -1) printf("Background process finished: %d\n", waitPid);
            if (waitPid == -1)
            {
                printf("Wait BG process fails\n");
            }
            else if (WIFSTOPPED(status)) { // stopped process (eg: SIGTTIN...)
                updateStatus(searchNode(&jobList, pointer->pid), STOPPED);
            } 
            else if (WIFSIGNALED(status)) { // process group killed by signal -> remove from job list
                deleteNode(&jobList, waitPid);
            } 
            else if (WIFEXITED(status))
            {
                pointer->numProcesses--;
                printf("Background process decrement job: %d\n", pointer->pid);
                // all processes in the job are done
                if (pointer->numProcesses == 0)
                {
                    printf("Background process finished: %d\n", waitPid);
                    pointer = pointer->next;
                    deleteNode(&jobList, waitPid);
                    continue;
                }
            }
            pointer = pointer->next;
        }

        tcsetpgrp(STDIN_FILENO, shellPid);
        // prompt and get user input
        char *userInput = malloc(maxLineLength);
        if (userInput == NULL)
        {
            perror("Cannot allocate memory for userInput");
            exit(EXIT_FAILURE);
        }

        int ret = getInput(&maxLineLength, userInput);
        if (ret == 1)
        {
            free(userInput);
            continue;
        }

        signalCaught = 0;

        // create parsed command struct from the user input
        struct parsed_command *cmd;
        int err = parse_command(userInput, &cmd);
        if (err < 0)
        {
            perror("parse_command");
            if (cmd != NULL)
                free(cmd);
            if (userInput != NULL)
                free(userInput);
            deleteList(&jobList);
            exit(EXIT_FAILURE);
        }

        if (err > 0)
        {
            printf("parser error: %d\n", err);
            perror("invalid");
            if (userInput != NULL)
                free(userInput);
            continue;
        }

        free(userInput);

        // optional: print out information of the command
        commandInfo(cmd);

        if (cmd->num_commands < 2)
            handleSingleCmd(cmd, userInput);
        else
            handleMultiCmd(cmd, userInput);

        int isBG = cmd->is_background ? 1 : 0;

        free(cmd);

        // repromt the user if the prev cmd is background
        if (isBG)
            continue;
    }

    deleteList(&jobList);
    return 0;
}

int getInput(size_t *maxLineLength, char *userInput)
{
    // write prompt
    write(STDERR_FILENO, PROMPT, strlen(PROMPT));

    // read in user input
    int numBytes = getline(&userInput, maxLineLength, stdin);
    int handleFlag = handleBytesRead(&numBytes, userInput);
    userInput[numBytes] = '\0';
    return handleFlag;
}

int handleBytesRead(int *numBytes, char *cmd)
{
    int numBytesRead = *numBytes;

    if (numBytesRead < 0)
    { // fail to read or ctrl D was hit at the beginning of the line
        free(cmd);
        deleteList(&jobList);
        exit(errno);
        // } else if (numBytesRead > 0 && cmd[numBytesRead-1] != '\n') { // ? need to handle: ctrl D was hit in the middle of the line --> YES
        //     printf("\n");
    }
    else if (numBytesRead == 1 && cmd[0] == '\n')
    { // empty input
        return 1;
        // } else if (numBytesRead < 0) {
        //     perror("Read failed");
        //     exit(EXIT_FAILURE);
    }

    return 0;
}

void handleSingleCmd(struct parsed_command *cmd, char *input)
{
    pid_t pid = fork();
    if (pid < 0)
        perror("fork error");
    else if (pid > 0)
    {                      /*parent process*/
        setpgid(pid, pid); // isolate child process
        pid_t cpgid = getpgid(pid);
        printf("child's pgid: %d\n", cpgid);

        if (cmd->is_background)
        {
            tcsetpgrp(STDIN_FILENO, shellPid); // hand tc to parent
            printf("BG child's pgid added to list: %d\n", cpgid);
            insertLast(&jobList, cpgid, cmd->num_commands, RUNNING, input);
        }
        else
        {
            tcsetpgrp(STDIN_FILENO, cpgid); // hand tc to child /foreground job
            int status = -1;
            pid_t waitPid = -1;

            while (!WIFEXITED(status) && !WIFSIGNALED(status))
            {
                waitPid = waitpid(cpgid, &status, WUNTRACED);
                if (waitPid == -1)
                {
                    perror("waitpid fails");
                    exit(EXIT_FAILURE);
                }

                if (WIFEXITED(status))
                {
                    printf("exited, status=%d\n", WEXITSTATUS(status));
                }
                else if (WIFSIGNALED(status))
                { // ctrl C terminates the child
                    printf("killed by signal %d\n", WTERMSIG(status));
                }
                else if (WIFSTOPPED(status))
                {
                    printf("stopped by signal %d\n", WSTOPSIG(status));
                    if (WSTOPSIG(status) == 20)
                    { // ctrl Z is hit -> stop child process + add to job list
                        insertLast(&jobList, cpgid, cmd->num_commands, STOPPED, input);
                        printNode(searchNode(&jobList, cpgid));
                        break;
                    }
                }
                else if (WIFCONTINUED(status))
                {
                    printf("continued\n");
                }
                else
                {
                    printf("break\n");
                    break;
                }
            }

            tcsetpgrp(STDIN_FILENO, shellPid); // hand tc back to parent
        }
    }
    else if (pid == 0)
    { /* child process */
        // standard input redirection
        if (cmd->stdin_file != NULL)
        {
            int newInputFd = open(cmd->stdin_file, O_RDONLY);

            // if fail to open, throw error + not execute the command
            if (newInputFd == -1)
            {
                perror("open failed\n");
                exit(EXIT_FAILURE);
            }

            int ret = dup2(newInputFd, STDIN_FILENO);
            if (ret < 0)
                perror("dup2 failed");
            close(newInputFd);
        }

        // standard output redirection
        if (cmd->stdout_file != NULL)
        {
            int newOutputFd;
            if (cmd->is_file_append)
            {
                newOutputFd = open(cmd->stdout_file, O_WRONLY | O_CREAT | O_APPEND | O_TRUNC, 0644);
            }
            else
            {
                newOutputFd = open(cmd->stdout_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            }

            // if fail to open, throw error + not execute the command
            if (newOutputFd == -1)
            {
                perror("open failed\n");
                exit(EXIT_FAILURE);
            }

            int ret = dup2(newOutputFd, STDOUT_FILENO);
            if (ret < 0)
                perror("dup2 failed");
            close(newOutputFd);
        }

        execvp(cmd->commands[0][0], cmd->commands[0]);
        exit(EXIT_FAILURE);
    }
}

void handleMultiCmd(struct parsed_command *cmd, char *input)
{
    pid_t pids[cmd->num_commands];
    // create n-1 pipes for n commands
    int fds[cmd->num_commands - 1][2];
    for (int i = 0; i < cmd->num_commands - 1; i++)
    {
        if (pipe(fds[i]) < 0)
        {
            perror("pipe error");
            exit(EXIT_FAILURE);
        }
    }

    pid_t cpgid = -1;

    for (int i = 0; i < cmd->num_commands; i++)
    {
        pids[i] = fork();
        if (pids[i] < 0)
        {
            perror("fork error");
            exit(EXIT_FAILURE);
        }

        // setpgid(pids[i], pids[0]);
        // printf("Pipe %d: %d\n", i, getpgid(pids[i]));

        // first process
        if (pids[i] == 0 && i == 0)
        {
            int ret;
            if (cmd->stdin_file)
            { // read in from new input file
                int newInputFd = open(cmd->stdin_file, O_RDONLY);

                // if fail to open, throw error + not execute the command
                if (newInputFd == -1)
                {
                    perror("open failed\n");
                    exit(EXIT_FAILURE);
                }

                ret = dup2(newInputFd, STDIN_FILENO);
                if (ret < 0)
                    perror("dup2 failed");
                close(newInputFd);
            }

            ret = dup2(fds[0][1], STDOUT_FILENO); // write to write end of the first pipe
            if (ret < 0)
                perror("dup2 failed");

            // clsoe all write pipes
            for (int j = 0; j < cmd->num_commands - 1; j++)
            {
                close(fds[j][1]);
            }

            execvp(cmd->commands[i][0], cmd->commands[i]);
        }

        // last process
        else if (pids[i] == 0 && i == cmd->num_commands - 1)
        {
            if (cmd->stdout_file != NULL)
            {
                int newOutputFd;
                if (cmd->is_file_append)
                {
                    newOutputFd = open(cmd->stdout_file, O_WRONLY | O_CREAT | O_APPEND | O_TRUNC, 0644);
                }
                else
                {
                    newOutputFd = open(cmd->stdout_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                }

                // if fail to open, throw error + not execute the command
                if (newOutputFd == -1)
                {
                    perror("open failed\n");
                    exit(EXIT_FAILURE);
                }

                int ret = dup2(newOutputFd, STDOUT_FILENO);
                if (ret < 0)
                    perror("dup2 failed");
                close(newOutputFd);
            }

            int ret = dup2(fds[i - 1][0], STDIN_FILENO); // read from read end of the pipe
            if (ret < 0)
                perror("dup2 failed");

            // close all write pipes
            for (int j = 0; j < cmd->num_commands - 1; j++)
            {
                close(fds[j][1]);
            }

            execvp(cmd->commands[i][0], cmd->commands[i]);
        }

        // middle process
        else if (pids[i] == 0 && i > 0 && i < cmd->num_commands - 1)
        {
            int ret = dup2(fds[i - 1][0], STDIN_FILENO); // read from stdin
            if (ret < 0)
                perror("dup2 failed");
            ret = dup2(fds[i][1], STDOUT_FILENO); // write to stdout
            if (ret < 0)
                perror("dup2 failed");

            // close all write pipes
            for (int j = 0; j < cmd->num_commands - 1; j++)
            {
                close(fds[j][1]);
            }
            execvp(cmd->commands[i][0], cmd->commands[i]);
        }

        if (pids[i] > 0)
        {                              // parent
            setpgid(pids[i], pids[0]); // isolate child process
            printf("child's pgid: %d\n", cpgid);
            cpgid = getpgid(pids[0]);
        }
    }

    /* parent process */
    // close all write pipes
    for (int j = 0; j < cmd->num_commands - 1; j++)
    {
        close(fds[j][0]);
        close(fds[j][1]);
    }

    // wait for child processes
    if (cmd->is_background)
    {
        tcsetpgrp(STDIN_FILENO, shellPid); // hand tc to parent
        printf("BG child's pgid added to list: %d\n", cpgid);
        insertLast(&jobList, cpgid, cmd->num_commands, RUNNING, input);
    }
    else
    {
        tcsetpgrp(STDIN_FILENO, cpgid); // hand tc to child /foreground job
        int status = -1;
        pid_t waitPid = -1;

        while (!WIFEXITED(status) && !WIFSIGNALED(status))
        {
            waitPid = waitpid(cpgid, &status, WUNTRACED);
            if (waitPid == -1)
            {
                perror("waitpid fails");
                exit(EXIT_FAILURE);
            }

            if (WIFEXITED(status))
            {
                printf("exited, status=%d\n", WEXITSTATUS(status));
            }
            else if (WIFSIGNALED(status))
            { // ctrl C terminates the child
                printf("killed by signal %d\n", WTERMSIG(status));
            }
            else if (WIFSTOPPED(status))
            {
                printf("stopped by signal %d\n", WSTOPSIG(status));
                if (WSTOPSIG(status) == 20)
                { // ctrl Z is hit -> stop child process + add to job list
                    insertLast(&jobList, cpgid, cmd->num_commands, STOPPED, input);
                    printNode(searchNode(&jobList, cpgid));
                    break;
                }
            }
            else if (WIFCONTINUED(status))
            {
                printf("continued\n");
            }
            else
            {
                printf("break\n");
                break;
            }
        }

        tcsetpgrp(STDIN_FILENO, shellPid); // hand tc back to parent
    }
    int childStatus;
    pid_t waitPid;
    for (int i = 0; i < cmd->num_commands; i++)
    {
        waitPid = waitpid(pids[i], &childStatus, 0);
        if (waitPid == -1)
        {
            perror("wait failed");
            exit(EXIT_FAILURE);
        }
    }
}

/** @brief Signal handler
 *  @param signum Int signal
 */
void signalHandler(int signum)
{

    if (signum == SIGINT || signum == SIGTSTP)
    { // handle Ctrl+C and Ctrl+Z
        printf("received SIGINT or SIGTSTP\n");
        signalCaught = 1;
        write(STDERR_FILENO, PROMPT, strlen(PROMPT));
    }
    else if (signum == SIGQUIT)
    {
        printf("received SIGQUIT\n");
    }
    else if (signum == SIGTTOU)
    {
        printf("received SIGTTOU\n");
    }
    else
    {
        exit(EXIT_FAILURE);
    }
}

void commandInfo(struct parsed_command *cmd)
{
    print_parsed_command(cmd);

    printf("Is background job:\t%s\n", cmd->is_background ? "true" : "false");
    printf("Is append output file:\t%s\n", cmd->is_file_append ? "true" : "false");

    printf("Input file name:\t%s\n", cmd->stdin_file);
    printf("Output file name:\t%s\n", cmd->stdout_file);
    printf("Number of commands:\t%zu\n", cmd->num_commands);

    for (int i = 0; i < cmd->num_commands; ++i)
    {
        printf("Pipeline %d:\t%p\n", i, cmd->commands[i]);
    }
}