/* The particle engine's intrusive list; see SPLList_Erase. */
typedef struct Node {
    struct Node *next; /* 0x0 */
    struct Node *prev; /* 0x4 */
} Node;

typedef struct List {
    Node *head; /* 0x0 */
    int count;  /* 0x4 */
} List;

/* Pop the head of list, or 0 when it is empty. */
Node *SPLList_PopFront(List *list) {
    Node *popped = 0;
    Node *head = list->head;
    if (head != 0) {
        popped = head;
        list->head = head->next;
        if (list->head != 0) {
            head->next->prev = 0;
        }
        list->count--;
    }
    return popped;
}
