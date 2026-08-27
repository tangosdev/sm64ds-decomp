extern char data_020a5634;
extern void func_0205abb8(int a, int b, int mask, int d);
extern void *func_0205afb4(void);
extern void func_0205b070(int v);
extern void func_0205afe0(void *p);

void func_02050258(void)
{
    char *G = &data_020a5634;
    int sh;
    int mask;
    void *p;
    if (*(int *)G == 0)
        return;
    sh = *(int *)(G + 0x28);
    if (sh >= 0)
        mask = 1 << sh;
    else
        mask = 0;
    func_0205abb8(*(int *)(G + 0x20), *(int *)(G + 0x24), mask, 0);
    p = func_0205afb4();
    func_0205b070(1);
    func_0205afe0(p);
}
