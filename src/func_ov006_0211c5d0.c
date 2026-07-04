extern void func_ov004_020adb1c(int arg);
extern void func_ov006_0211c478(int self);
extern void func_ov006_0211c080(int self);
extern void func_ov004_020b0cac(int c, int a1, int a2, int a3, int arg5, short arg6);
extern void _ZN5Sound22LoadAndSetMusic_Layer1Ei(int n);

extern int *g_020beb68;

void func_ov006_0211c5d0(int self, int r1)
{
    int *p;
    int *q;
    int *base;
    int v;

    *(int*)(self + 0x5608) = 0;

    base = (int*)self;
    if (r1 == 0) {
        q = (int*)(((int)self + 0xbc) & 0xFFFFFFFFFFFFFFFF);
        *q = *q + 1;
        if ((unsigned int)base[0xbc/4] > 0x270e)
            base[0xbc/4] = 0x270e;
    } else {
        base[0xbc/4] = 0;
        if ((unsigned int)base[0xbc/4] > 0x270e)
            base[0xbc/4] = 0x270e;

        p = g_020beb68;
        if (p)
            *(int*)((int)p + 0xb4) = 0;

        p = g_020beb68;
        if (p)
            v = *(int*)((int)p + 0xb4);
        else
            v = 0;
        func_ov004_020adb1c(v);
    }

    *(int*)(self + 0x50e0) = 1;
    func_ov006_0211c478(self);

    *(signed char*)(self + 0x5626) = 3;
    func_ov006_0211c080(self);

    *(short*)(self + 0x5618) = 0x20;
    func_ov004_020b0cac(0xd, 0x80, 0xa8, 1, -1, 0xd);

    _ZN5Sound22LoadAndSetMusic_Layer1Ei(6);
}
