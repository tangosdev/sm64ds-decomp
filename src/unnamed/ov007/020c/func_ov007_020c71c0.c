extern void func_ov007_020c80c8(int x, int b, int c);

struct S {
    int n;       /* [0]  inner bound */
    int m;       /* [4]  outer bound */
    int ***tbl;  /* [8] */
};

void func_ov007_020c71c0(struct S *s, int b, int c)
{
    int j, i;
    for (i = 0; i < s->m; i++)
        for (j = 0; j < s->n; j++)
            func_ov007_020c80c8(s->tbl[i][0][j], b, c);
}
