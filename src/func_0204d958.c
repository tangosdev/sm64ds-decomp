typedef struct Node {
    struct Node *next;   /* 0x0 */
    struct Node *prev;   /* 0x4 */
} Node;

typedef struct List {
    Node *head;   /* 0x0 */
    int count;    /* 0x4 */
} List;

Node *func_0204d958(List *list)
{
    Node *popped = 0;
    Node *head = list->head;
    if (head != 0) {
        popped = head;
        list->head = head->next;
        if (list->head != 0) {
            head->next->prev = 0;
        }
        {
            int *cp = (int *)(((int)list + 4));
            *cp = *cp - 1;
        }
    }
    return popped;
}
