struct N {
    struct N *f0;
    struct N *f4;
};

extern int func_0203b20c(struct N *l, struct N *n);

int func_0203b2ec(struct N *l, struct N *p, struct N *n)
{
    struct N *t;
    if (n == 0)
        return func_0203b20c(l, p);
    if (p == 0)
        goto ret0;
    p->f4 = n->f4;
    p->f0 = n;
    n->f4 = p;
    t = p->f4;
    if (t != 0)
        t->f0 = p;
    else
        l->f4 = p;
    goto ret1;
ret0:
    return 0;
ret1:
    return 1;
}
