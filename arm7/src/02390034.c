extern void fn_0238E610(unsigned short *);
extern void fn_02382C98(void *, void *, unsigned int);

unsigned short *fn_02390034(unsigned short *a1, short a2, short a3, short a4, void *a5)
{
    unsigned short *v6;
    a1[0] = 0;
    a1[1] = 0;
    a1[2] = 0;
    a1[3] = 0;
    a1[4] = 0;
    a1[5] = 0;
    a1[6] = 0xA;
    a1[7] = 0xC;
    a1[8] = 0;
    a1[9] = a2;
    a1[10] = a3;
    a1[11] = a4;
    fn_02382C98(a5, a1 + 12, 0x10);
    v6 = a1 + a1[7] + 8;
    *v6 = a1[6];
    v6[1] = 0x12;
    fn_0238E610(a1);
    return v6;
}
