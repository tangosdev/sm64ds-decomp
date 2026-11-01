extern void fn_0238E610(unsigned short *);
extern int fn_0238D594(int, int);
extern void fn_02382C98(void *, void *, unsigned int);

unsigned short *fn_023900C0(unsigned short *a1, short a2, void *a3, short a4,
                   unsigned short a5, unsigned short a6, unsigned short a7,
                   unsigned short a8, unsigned short a9, void *a10)
{
    unsigned short *v14;
    a1[0] = 0;
    a1[1] = 0;
    a1[2] = 0;
    a1[3] = 0;
    a1[4] = 0;
    a1[5] = 0;
    a1[6] = 9;
    a1[7] = fn_0238D594(a9 + 1, 2) + 0x17;
    a1[8] = a2;
    fn_02382C98(a3, a1 + 9, 0x20);
    a1[25] = a4;
    a1[26] = a5;
    a1[27] = a6;
    a1[28] = a7;
    a1[29] = a8;
    a1[30] = a9;
    fn_02382C98(a10, a1 + 31, a9);
    v14 = a1 + a1[7] + 8;
    *v14 = a1[6];
    v14[1] = 1;
    fn_0238E610(a1);
    return v14;
}
