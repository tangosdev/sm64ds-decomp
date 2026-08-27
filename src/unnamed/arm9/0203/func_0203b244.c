struct N { struct N* prev; struct N* next; };
struct L { struct N* head; struct N* tail; };
int func_0203b244(struct L* l, struct N* n){
    struct N* old;
    if (n != 0){
        old = l->tail;
        if (old != 0){
            old->next = n;
            n->prev = l->tail;
        } else {
            l->head = n;
        }
        l->tail = n;
    } else {
        return 0;
    }
    return 1;
}
