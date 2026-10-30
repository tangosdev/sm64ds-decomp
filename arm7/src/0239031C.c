extern void fn_02382C98(void *src, void *dst, int n);
extern void fn_0238E610(unsigned short *a1);

unsigned short *fn_0239031C(unsigned short *a1, unsigned short a2, void *src) {
    unsigned short zero = 0;
    a1[0] = zero;
    a1[1] = zero;
    a1[2] = zero;
    a1[3] = zero;
    a1[4] = zero;
    a1[5] = zero;
    a1[6] = 3;
    a1[7] = 0x22;
    a1[8] = a2;
    a1[9] = zero;
    fn_02382C98(src, a1 + 10, 0x44);
    {
        unsigned short *slot = &a1[a1[7]] + 8;
        slot[0] = a1[6];
        slot[1] = 5;
        fn_0238E610(a1);
        return slot;
    }
}
