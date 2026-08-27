extern void func_02068834(int v);
extern int func_02062240(int a, int b, int c);
extern int func_02061ce4(int x);
extern char *data_020a9db4;
extern unsigned short data_020a9db0;

int func_02068d08(void)
{
    char *base;
    int ret;
    base = data_020a9db4;
    *(short *)(base + 0x1e28) = 0;
    *(short *)(data_020a9db4 + 0x1e2a) = 0;
    *(short *)(data_020a9db4 + 0x1e26) = 0;
    *(short *)(data_020a9db4 + 0x1e48) = 0;
    func_02068834(0xa);
    do {
        ret = func_02062240((int)data_020a9db4,
                            *(int *)(data_020a9db4 + 0x140c),
                            data_020a9db0);
    } while (ret == 4);
    if (ret != 2) return 8;
    func_02061ce4(*(int *)(data_020a9db4 + 0x140c));
    *(unsigned char *)(data_020a9db4 + 0x1411) = 1;
    return 0;
}
