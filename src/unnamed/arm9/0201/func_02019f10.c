extern char data_02086214[];
extern void func_02019f64(void);
extern void func_02058200(void *a, void *fn, int c, void *d, int e, int f);
extern void func_02058048(void *a);

int func_02019f10(void)
{
    char *base = (char *)((int)data_02086214 - 0x94);
    func_02058200(base, (void *)func_02019f64, 0, base, 0x3f6c, 0);
    func_02058048(base);
    return 0;
}
