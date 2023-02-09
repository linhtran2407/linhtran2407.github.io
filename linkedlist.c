#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linkedlist.h"

// Insert at the beginning
void insertFirst(struct node **head_ref, int new_data, int num_cmds, char *status, char *cmd)
{
    // Allocate memory to a node
    struct node *new_node = (struct node *)malloc(sizeof(struct node));

    // insert the data
    new_node->pid = new_data;
    new_node->numProcesses = num_cmds;
    strcpy(new_node->command, cmd);

    int i;
    for (i = 0; i < strlen(status); i++)
    {
        new_node->status[i] = status[i];
    }
    new_node->status[i] = '\0';
    new_node->next = (*head_ref);

    // Move head to new node
    (*head_ref) = new_node;
}

// Insert the the end
void insertLast(struct node **head_ref, int new_data, int num_cmds, char *status, char *cmd)
{
    struct node *new_node = (struct node *)malloc(sizeof(struct node));
    struct node *last = *head_ref;

    new_node->pid = new_data;
    new_node->numProcesses = num_cmds;
    strcpy(new_node->command, "Test command");
    printf("%s\n", new_node->command);

    int i;
    for (i = 0; i < strlen(status); i++)
    {
        new_node->status[i] = status[i];
    }
    new_node->status[i] = '\0';
    new_node->next = NULL;

    if (*head_ref == NULL)
    {
        *head_ref = new_node;
        return;
    }

    while (last->next != NULL)
        last = last->next;

    last->next = new_node;

    return;
}

// Delete a node
void deleteNode(struct node **head_ref, int key)
{
    if (head_ref == NULL)
        return;
    struct node *temp = *head_ref, *prev;

    if (temp != NULL && temp->pid == key)
    {
        *head_ref = temp->next;
        free(temp);
        return;
    }
    // Find the key to be deleted
    while (temp != NULL && temp->pid != key)
    {
        prev = temp;
        temp = temp->next;
    }

    // If the key is not present
    if (temp == NULL || prev == NULL)
        return;

    // Remove the node
    prev->next = temp->next;

    free(temp);
}

void deleteList(struct node **head_ref)
{
    if (head_ref == NULL || (*head_ref) == NULL)
        return;
    struct node *current = *head_ref;

    while (current != NULL)
    {
        struct node *temp = current->next;
        free(current);
        current = temp;
    }
}

void printList(struct node *node)
{
    printf("Job list: [");

    while (node != NULL)
    {
        printf("Cmd: %s, pid: %d - numProcesses: %d (%s), ", node->command, node->pid, node->numProcesses, node->status);
        node = node->next;
    }
    printf("]\n");
}

int size(struct node *node)
{
    int count = 0;

    while (node != NULL)
    {
        count++;
        node = node->next;
    }

    return count;
}

struct node *searchNode(struct node **head_ref, int key)
{
    struct node *current = *head_ref;

    while (current != NULL)
    {
        if (current->pid == key)
            return current;
        current = current->next;
    }

    return NULL;
}

void printNode(struct node *current)
{
    if (current == NULL)
    {
        return;
    }
    printf("(pid: %d, numProcesses: %d, status: %s), command: %s\n", current->pid, current->numProcesses, current->status, current->command);
}

// update status of a process
// return 1 if success, 0 otherwise
int updateStatus(struct node *current, char *new_status)
{
    if (current != NULL)
    {
        int i;
        for (i = 0; i < strlen(new_status); i++)
        {
            current->status[i] = new_status[i];
        }
        current->status[i] = '\0';
        return 1;
    }

    return 0;
}