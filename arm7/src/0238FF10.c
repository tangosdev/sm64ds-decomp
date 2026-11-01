extern void fn_0238E610(unsigned short *);

unsigned short *fn_0238FF10(unsigned short *a1, short a2, short a3, int a4)
{
    unsigned short *v4;
    a1[0] = 0;
    a1[1] = 0;
    a1[2] = 0;
    a1[3] = 0;
    a1[4] = 0;
    a1[5] = 0;
    a1[6] = 0x101;
    a1[7] = 4;
    a1[8] = a2;
    a1[9] = a3;
    *(int *)(a1 + 10) = a4;
    v4 = a1 + a1[7] + 8;
    *v4 = a1[6];
    v4[1] = 1;
    fn_0238E610(a1);
    return v4;
}
