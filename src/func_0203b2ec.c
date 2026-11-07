struct N {
    struct N *prev;
    struct N *next;
};
struct L {
    struct N *head;
    struct N *tail;
};

extern int func_0203b20c(struct L *l, struct N *n);

int func_0203b2ec(struct L *l, struct N *p, struct N *n) {
    struct N *t;
    if (n == 0)
        return func_0203b20c(l, p);
    if (p != 0) {
        p->next = n->next;
        p->prev = n;
        n->next = p;
        t = p->next;
        if (t != 0)
            t->prev = p;
        else
            l->tail = p;
    } else {
        return 0;
    }
    return 1;
}
