extern void fn_02382C98(void *src, void *dst, int n);
extern void fn_0238E610(unsigned short *a1);

unsigned short *fn_023903A0(unsigned short *a1, unsigned a2, void *a3, unsigned short a4,
                            void *a5, unsigned short a6, void *a7, unsigned short a8) {
    a1[0] = 0;
    a1[1] = 0;
    a1[2] = 0;
    a1[3] = 0;
    a1[4] = 0;
    a1[5] = 0;
    a1[6] = 2;
    a1[7] = 0x1F;
    fn_02382C98(a3, a1 + 8, 6);
    a1[11] = a4;
    fn_02382C98(a5, a1 + 12, 0x20);
    a1[28] = a6;
    fn_02382C98(a7, a1 + 29, 0x10);
    a1[37] = a8;
    a1[38] = 0;
    {
        unsigned short *slot = &a1[a1[7]] + 8;
        slot[0] = a1[6];
        slot[1] = (a2 >> 1) - 0x2C;
        fn_0238E610(a1);
        return slot;
    }
}
