extern void func_ov007_020c8440(void* p);
extern void func_ov007_020c3d1c(void* p);
extern void func_ov007_020c8098(void* p);

struct S {
    void** a0;      /* 0x0 */
    int n;          /* 0x4 */
    void** a8;      /* 0x8 */
    int flags;      /* 0xc */
};

void func_ov007_020c78dc(struct S* c)
{
    int i;
    if (c->flags & 1) {
        for (i = 0; i < c->n; i++)
            func_ov007_020c8440(c->a0[i]);
    }
    func_ov007_020c3d1c(c->a0);
    if (c->n > 1) {
        int m;
        if (c->n <= 1)
            m = 0;
        else
            m = c->n - ((c->flags & 2) ? 0 : 1);
        for (i = 0; i < m; i++)
            func_ov007_020c8098(c->a8[i]);
        func_ov007_020c3d1c(c->a8);
    }
    func_ov007_020c3d1c(c);
}
