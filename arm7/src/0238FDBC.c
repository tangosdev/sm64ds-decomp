extern void fn_0238E610(unsigned short *);

unsigned short *fn_0238FDBC(unsigned short *a1, short a2)
{
    unsigned short *v2;
    a1[0] = 0;
    a1[1] = 0;
    a1[2] = 0;
    a1[3] = 0;
    a1[4] = 0;
    a1[5] = 0;
    a1[6] = 0x204;
    a1[7] = 1;
    a1[8] = a2;
    v2 = a1 + a1[7] + 8;
    *v2 = a1[6];
    v2[1] = 1;
    fn_0238E610(a1);
    return v2;
}
