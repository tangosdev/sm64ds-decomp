extern void fn_02382C98(void *src, void *dst, int n);
extern void fn_0238E610(unsigned short *a1);

unsigned short *fn_02390214(unsigned short *a1, void *src, unsigned short a3) {
    a1[0] = 0;
    a1[1] = 0;
    a1[2] = 0;
    a1[3] = 0;
    a1[4] = 0;
    a1[5] = 0;
    a1[6] = 5;
    a1[7] = 4;
    fn_02382C98(src, a1 + 8, 6);
    a1[11] = a3;
    {
        unsigned short *slot = &a1[a1[7]] + 8;
        slot[0] = a1[6];
        slot[1] = 4;
        fn_0238E610(a1);
        return slot;
    }
}
