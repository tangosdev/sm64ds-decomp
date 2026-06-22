struct Node88 {
    char pad[0xc];
    void *fc;
    void *f10;
};

struct Obj88 {
    char pad[0x7c];
    struct Node88 *f7c;
    struct Node88 *f80;
};

void func_020589ac(struct Obj88 *o, struct Node88 *n) {
    struct Node88 *t = o->f80;
    if (t == 0) o->f7c = n;
    else o->f7c->f10 = n;
    n->fc = t;
    n->f10 = 0;
    o->f80 = n;
}
