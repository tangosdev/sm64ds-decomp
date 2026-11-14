#pragma opt_propagation off

extern void fn_0238E610(unsigned short *a1);

unsigned short *fn_0238FA3C(unsigned short *a1, unsigned short a2, unsigned short a3) {
    unsigned short zero;

    zero = 0;
    a1[0] = zero;
    a1[1] = zero;
    a1[2] = zero;
    a1[3] = zero;
    a1[4] = zero;
    a1[5] = zero;
    a1[6] = a2;
    a1[7] = zero;
    {
        unsigned short *slot = &a1[a1[7]] + 8;
        slot[0] = a1[6];
        slot[1] = a3;
        fn_0238E610(a1);
        return slot;
    }
}
