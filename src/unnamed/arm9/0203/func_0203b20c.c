struct N { struct N* prev; struct N* next; };
struct L { struct N* head; struct N* tail; };
int func_0203b20c(struct L* l, struct N* n){
    struct N* old;
    if (n != 0){
        old = l->head;
        if (old != 0){
            old->prev = n;
            n->next = l->head;
        } else {
            l->tail = n;
        }
        l->head = n;
    } else {
        return 0;
    }
    return 1;
}
