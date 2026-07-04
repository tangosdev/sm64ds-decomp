extern void func_ov004_020adb1c(int self);
extern void func_ov004_020b0aa0(int arg);
extern void func_ov006_021067a4(char *p);
extern void func_ov006_021063a0(char *p);
extern void func_ov006_02106168(char *p);
extern void func_ov006_02104b24(char *p);
extern void func_ov004_020b0cac(int c, int a1, int a2, int a3, int arg5, short arg6);
extern void SetSubBg0Offset(int a, int b);

extern char *func_020beb68;
extern unsigned char data_0209d454;

void func_ov006_021071fc(char *self, int flag)
{
    char *p;

    if (flag == 0) {
        int *q = (int *)(((int)self + 0xbc) & 0xFFFFFFFFFFFFFFFF);
        *q += 1;
        if (*(unsigned int *)(self + 0xbc) > 0x270e)
            *(int *)(self + 0xbc) = 0x270e;
    } else {
        *(unsigned char *)(self + 0x4fea) = 0;
        *(int *)(self + 0xbc) = 0;
        if (*(unsigned int *)(self + 0xbc) > 0x270e)
            *(int *)(self + 0xbc) = 0x270e;

        if (func_020beb68 != 0)
            *(int *)(func_020beb68 + 0xb4) = 0;

        p = func_020beb68;
        func_ov004_020adb1c(p != 0 ? *(int *)(p + 0xb4) : 0);
    }

    func_ov004_020b0aa0(0x1d);
    func_ov006_021067a4(self);
    func_ov006_021063a0(self);
    func_ov006_02106168(self);
    func_ov006_02104b24(self);

    *(unsigned char *)(self + 0x4fe2) = 3;
    *(int *)(self + 0x4ca8) = 0;
    func_ov004_020b0cac(0xd, 0x80, 0x40, 0, -1, 0xd);
    SetSubBg0Offset(0, 0);

    data_0209d454 &= ~1;
}
