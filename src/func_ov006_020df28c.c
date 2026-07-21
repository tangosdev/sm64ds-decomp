    typedef unsigned char u8;
    
    extern void func_ov006_020def80(void *self, int i);
    extern void func_ov006_020c2594(void *c);
    extern void func_ov004_020b1b78(void *c, int val);
    extern void func_ov006_020c2664(char *c);
    extern void func_ov006_020c2440(char *c);
    extern void func_02012790(int x);
    extern void func_ov004_020adb1c(int self);
    
    #pragma opt_strength_reduction off
    void func_ov006_020df28c(char *self)
    {
        int r4;
        int i;
        int *pc = (int *)(((int)self + 0x541c) & 0xFFFFFFFFFFFFFFFFLL);
        *pc = *pc - 1;
        if (*(int *)(self + 0x5000 + 0x41c) > 0) return;
        for (i = 0; i < 3; i++) {
            u8 *q = (u8 *)(((int)self + i + 0x5465) & 0xFFFFFFFFFFFFFFFFLL);
            if (*q == 0) {
                *q = 1;
                func_ov006_020def80(self, i);
            }
        }
        r4 = *(int *)(self + 0xb4);
        if (*(u8 *)(self + 0x5000 + 0x469) != 0) {
            *(int *)(self + 0x5000 + 0x41c) = 0x3c;
            func_ov006_020c2594(self + 0x4f38);
            r4++;
        } else {
            func_ov004_020b1b78(self, 1);
            if (*(int *)(self + 0xa8) > 0) {
                *(int *)(self + 0x5000 + 0x41c) = 0x96;
                func_ov006_020c2664(self + 0x4f38);
            } else {
                *(int *)(self + 0x5000 + 0x41c) = 0x1e;
                func_ov006_020c2440(self + 0x4f38);
            }
            func_02012790(0x12f);
        }
        func_ov004_020adb1c(r4);
        *(int *)(self + 0x5000 + 0x418) = 6;
        *(u8 *)(self + 0xc3) = 0;
    }
