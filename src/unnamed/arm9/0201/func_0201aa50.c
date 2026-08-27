extern int func_02059bb0(void);
extern int func_020599e8(void *a, void *b, void *c, void *d, int e);

int func_0201aa50(void *a, void *b, void *c, void *d, int e)
{
    int r = func_02059bb0();
    if (r == 0)
        return r;
    return func_020599e8(a, b, c, d, e);
}
