extern void fn_0238E610(unsigned short *);

unsigned short *fn_0238FE74(unsigned short *a1, short a2, short a3, short a4,
                   unsigned short a5, unsigned short a6, unsigned short a7,
                   unsigned short a8, unsigned short a9, int a10)
{
    unsigned short *v10;
    a1[0] = 0;
    a1[1] = 0;
    a1[2] = 0;
    a1[3] = 0;
    a1[4] = 0;
    a1[5] = 0;
    a1[6] = 0x102;
    a1[7] = 0xA;
    a1[8] = a2;
    a1[9] = a3;
    a1[10] = a4;
    a1[11] = a5;
    a1[12] = a6;
    a1[13] = a7;
    a1[14] = a8;
    a1[15] = a9;
    *(int *)(a1 + 16) = a10;
    v10 = a1 + a1[7] + 8;
    *v10 = a1[6];
    v10[1] = 1;
    fn_0238E610(a1);
    return v10;
}
