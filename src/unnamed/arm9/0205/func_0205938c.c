struct Node { struct Node *prev; struct Node *next; };

struct Node *func_0205938c(struct Node *next, struct Node *node)
{
    node->next = next;
    node->prev = 0;
    if (next)
        next->prev = node;
    return node;
}
