struct Node {
    char pad[0xc];
    struct Node *prev; /* 0xc */
    struct Node *next; /* 0x10 */
};
struct List {
    char pad[0x7c];
    struct Node *head; /* 0x7c */
    struct Node *tail; /* 0x80 */
};
void func_02058988(struct List *list, struct Node *node)
{
    struct Node *next = node->next;
    struct Node *prev = node->prev;
    if (next == 0)
        list->tail = prev;
    else
        next->prev = prev;
    if (prev == 0)
        list->head = next;
    else
        prev->next = next;
}
