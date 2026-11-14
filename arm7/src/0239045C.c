#pragma scheduling off
#pragma opt_propagation off

extern void fn_0238E610(unsigned short *a1);

unsigned short *fn_0239045C(unsigned short *a1, unsigned short a2, unsigned short a3, unsigned short a4) {
    unsigned short zero = 0;
    unsigned short one;
    a1[0] = zero;
    a1[1] = zero;
    a1[2] = zero;
    a1[3] = zero;
    a1[4] = zero;
    a1[5] = zero;
    one = 1;
    a1[6] = one;
    a1[7] = 3;
    a1[8] = a2;
    a1[9] = a3;
    a1[10] = a4;
    {
        unsigned short *slot = &a1[a1[7]] + 8;
        slot[0] = a1[6];
        slot[1] = one;
        fn_0238E610(a1);
        return slot;
    }
}
