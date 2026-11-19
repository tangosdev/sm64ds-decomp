/* The particle engine's two-word intrusive list (SPLList_PopFront pops,
   SPLList_PushFront pushes): each element starts with next/prev links, whatever
   its type (Particle::System, ParticleNode, a free-pool node). */
typedef struct Node {
    struct Node *next; /* 0x0 */
    struct Node *prev; /* 0x4 */
} Node;

typedef struct List {
    Node *head; /* 0x0 */
    int count;  /* 0x4 */
} List;

/* Unlink n from l and return it. */
Node *SPLList_Erase(List *l, Node *n) {
    if (n->next == 0) {
        if (l->head == n)
            l->head = 0;
        else
            n->prev->next = 0;
    } else {
        if (l->head == n) {
            l->head = n->next;
            l->head->prev = 0;
        } else {
            n->next->prev = n->prev;
            n->prev->next = n->next;
        }
    }
    l->count--;
    return n;
}
