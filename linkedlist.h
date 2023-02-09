struct node
{
  int pid;
  int numProcesses;
  char status[10]; // either stopped or running -> need at most 10 chars
  struct node *next;
  char command[4096];
};

void printList(struct node *node);
void insertFirst(struct node **head_ref, int new_data, int num_cmds, char *status, char *cmd);
void insertLast(struct node **head_ref, int new_data, int num_cmds, char *status, char *cmd);
void deleteNode(struct node **head_ref, int key);
void deleteList(struct node **head_ref);
int size(struct node *node);
void deleteList(struct node **head_ref);
struct node *searchNode(struct node **head_ref, int key);
void printNode(struct node *current);
int updateStatus(struct node *node, char *new_status);
