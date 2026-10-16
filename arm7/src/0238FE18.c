extern void fn_0238E610(unsigned short *);

unsigned short *fn_0238FE18(unsigned short *a1)
{
    unsigned short *v1;
    a1[0] = 0;
    a1[1] = 0;
    a1[2] = 0;
    a1[3] = 0;
    a1[4] = 0;
    a1[5] = 0;
    a1[6] = 0x200;
    a1[7] = 0x48;
    v1 = a1 + a1[7] + 8;
    *v1 = a1[6];
    v1[1] = 1;
    fn_0238E610(a1);
    return v1;
}
