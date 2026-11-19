/* The particle engine's intrusive list; see SPLList_Erase. */
typedef struct Node {
    struct Node *next; /* 0x0 */
    struct Node *prev; /* 0x4 */
} Node;

typedef struct List {
    Node *head; /* 0x0 */
    int count;  /* 0x4 */
} List;

/* Push node onto the front of list. */
void SPLList_PushFront(List *list, Node *node) {
    Node *old = list->head;
    if (old == 0) {
        list->head = node;
        node->prev = node->next = 0;
    } else {
        node->next = old;
        node->prev = 0;
        list->head->prev = node;
        list->head = node;
    }
    list->count++;
}
