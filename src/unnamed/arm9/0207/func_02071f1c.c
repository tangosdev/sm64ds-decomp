extern void func_02072168(void *a, void *b, int d);
extern void func_02071790(void *a, void *b, int x);

void func_02071f1c(char *a, void *b, char *c, int d)
{
    char *buf;
    int off;

    func_02072168(a, b, d);

    off = *(int *)(c + 8);
    buf = *(char **)(a + 0x18) + off;
    *(int *)(*(char **)(a + 0x18) + off) = *(int *)(a + 4);
    *(int *)(buf + 4) = *(int *)a;
    *(int *)(buf + 8) = *(int *)(a + 8);
    *(int *)(buf + 0x14) = d;

    func_02071790(a, b, *(int *)b + *(int *)(c + 4));
}
