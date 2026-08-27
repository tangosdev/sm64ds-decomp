struct Node { struct Node *prev; struct Node *next; };
void func_0205d920(struct Node *node)
{
    struct Node *prev = node->prev;
    struct Node *next = node->next;
    if (prev) prev->next = next;
    if (next) next->prev = prev;
    node->prev = 0;
    node->next = node->prev;
}
