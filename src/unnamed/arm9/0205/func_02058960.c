struct Node87 {
    char pad[0xc];
    void *fc;
    void *f10;
};

struct Obj87 {
    char pad[0x7c];
    struct Node87 *f7c;
    void *f80;
};

struct Node87 *func_02058960(struct Obj87 *o) {
    struct Node87 *cur = o->f7c;
    struct Node87 *nxt = cur->f10;
    if (nxt == 0) o->f80 = 0;
    else nxt->fc = 0;
    o->f7c = nxt;
    return cur;
}
