extern void fn_0238E610(unsigned short *);
extern void fn_02382C98(void *, void *, unsigned int);

unsigned short *fn_02390194(unsigned short *a1, void *a2, short a3, short a4)
{
    unsigned short *v7;
    a1[0] = 0;
    a1[1] = 0;
    a1[2] = 0;
    a1[3] = 0;
    a1[4] = 0;
    a1[5] = 0;
    a1[6] = 6;
    a1[7] = 5;
    fn_02382C98(a2, a1 + 8, 6);
    a1[11] = a3;
    a1[12] = a4;
    v7 = a1 + a1[7] + 8;
    *v7 = a1[6];
    v7[1] = 3;
    fn_0238E610(a1);
    return v7;
}
