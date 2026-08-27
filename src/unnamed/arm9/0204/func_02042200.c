extern char data_020a11e4[];
extern void func_02042254(void);
extern void func_02058200(void *a, void *fn, void *c, void *d, int e, int f);
extern void func_02058048(void *a);

void func_02042200(void)
{
    char *base = (char *)(int)data_020a11e4;
    *(short *)(base + 0x494) = 0;
    func_02058200(base + 0x400, (void *)func_02042254, base, base + 0x400, 0x400, 0xf);
    func_02058048(base + 0x400);
}
