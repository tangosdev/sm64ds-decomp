struct Node { struct Node *next; char pad[8]; void *item; };

void *func_0203b358(struct Node *thiz)
{
    struct Node *n;
    if (thiz->item) return thiz->item;
    n = thiz->next;
    while (n) {
        if (n->item) return n->item;
        n = n->next;
    }
    return 0;
}
