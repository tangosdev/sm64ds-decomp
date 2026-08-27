struct Node {
    struct Node* prev;
    struct Node* next;
};

struct List {
    struct Node* tail;
    int count;
};

void func_0204d9a0(struct List* list, struct Node* node)
{
    struct Node* old = list->tail;
    if (old == 0) {
        list->tail = node;
        node->next = node->prev = 0;
    } else {
        node->prev = old;
        node->next = 0;
        list->tail->next = node;
        list->tail = node;
    }
    *(int*)(((int)list + 4)) += 1;
}
