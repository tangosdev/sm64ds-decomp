extern void func_ov007_020c78dc(int x);
extern void func_ov007_020c3d1c(void *p);

struct S {
    int n;        /* [0] */
    int m;        /* [4] */
    int *a;       /* [8] */
    int *b;       /* [0xc] */
};

void func_ov007_020c7368(struct S *s)
{
    int i;
    for (i = 0; i < s->m; i++)
        func_ov007_020c78dc(s->a[i]);
    for (i = 0; i < s->n; i++)
        func_ov007_020c78dc(s->b[i]);
    func_ov007_020c3d1c(s->a);
    func_ov007_020c3d1c(s->b);
    func_ov007_020c3d1c(s);
}
