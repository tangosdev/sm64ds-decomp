extern void fn_02382C98(void *src, void *dst, int n);
extern void fn_0238E610(unsigned short *a1);

unsigned short *fn_02390298(unsigned short *a1, void *src, unsigned short a3, unsigned short a4) {
    a1[0] = 0;
    a1[1] = 0;
    a1[2] = 0;
    a1[3] = 0;
    a1[4] = 0;
    a1[5] = 0;
    a1[6] = 4;
    a1[7] = 5;
    fn_02382C98(src, a1 + 8, 6);
    a1[11] = a3;
    a1[12] = a4;
    {
        unsigned short *slot = &a1[a1[7]] + 8;
        slot[0] = a1[6];
        slot[1] = 6;
        fn_0238E610(a1);
        return slot;
    }
}
