extern void func_ov004_020adb1c(int self);
extern void func_ov004_020b0aa0(int arg);
extern void func_ov006_020f2ec0(char *c);
extern void func_ov006_020f2e20(char *c);
extern void func_ov004_020b0cac(int c, int a1, int a2, int a3, int arg5, short arg6);

extern char *func_020beb68;
extern unsigned char data_0209d45c;
extern unsigned char data_0209d454;

void func_ov006_020f3294(char *c, int arg1)
{
    char *p;
    int *q;

    if (*(unsigned char *)(c + 0x5000 + 0x459) != 0) {
        *(unsigned char *)(((int)c + 0x5457) & 0xFFFFFFFFFFFFFFFF) += 1;

        q = (int *)(((int)c + 0xbc) & 0xFFFFFFFFFFFFFFFF);
        *q += 1;
        if ((unsigned int)*(int *)(c + 0xbc) > 0x270e)
            *(int *)(c + 0xbc) = 0x270e;
    }

    if (arg1 == 0x12) {
        *(short *)(c + 0x5100 + 0x72) = 0xa;
        *(int *)(c + 0xbc) = 0;
        if ((unsigned int)*(int *)(c + 0xbc) > 0x270e)
            *(int *)(c + 0xbc) = 0x270e;

        if (func_020beb68 != 0)
            *(int *)(func_020beb68 + 0xb4) = 0;

        p = func_020beb68;
        func_ov004_020adb1c(p != 0 ? *(int *)(p + 0xb4) : 0);
    }

    func_ov004_020b0aa0(0x1d);
    func_ov006_020f2ec0(c);
    func_ov006_020f2e20(c);

    *(int *)(c + 0x4000 + 0xf78) = 0;
    data_0209d45c |= 4;
    data_0209d454 &= ~4;

    func_ov004_020b0cac(0xd, 0x80, 0xa8, 1, -1, 0xd);
}
