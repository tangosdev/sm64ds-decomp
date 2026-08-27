struct N {
    struct N *f0;   /* 0x0 */
    struct N *f4;   /* 0x4 */
    struct N *f8;   /* 0x8 */
    struct N *fc;   /* 0xc */
};

int func_0203b438(struct N *a, struct N *b, struct N *c)
{
    struct N *t;
    struct N *n;
    if (b == 0) goto ret0;
    if (c == 0) goto handle_a;
    b->f0 = c;
    t = c->f4;
    if (t == 0) {
        c->f4 = b;
        goto ret1;
    }
    n = t->fc;
    while (n != 0) {
        t = n;
        n = n->fc;
    }
    t->fc = b;
    b->f8 = t;
    goto ret1;
handle_a:
    if (a->f0 != 0) return 0;
    a->f0 = b;
    goto ret1;
ret0:
    return 0;
ret1:
    return 1;
}
