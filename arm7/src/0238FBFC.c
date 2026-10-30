extern void fn_0238E688(void);
extern void fn_0238E610(unsigned short *);
extern int fn_0238D594(int, int);
extern void fn_02382C98(void *, void *, unsigned int);

unsigned short *fn_0238FBFC(unsigned short *a1, short a2, void *a3)
{
    unsigned short *v6;
    fn_0238E688();
    a1[0] = 0;
    a1[1] = 0;
    a1[2] = 0;
    a1[3] = 0;
    a1[4] = 0;
    a1[5] = 0;
    a1[6] = 0x245;
    a1[7] = fn_0238D594(a2 + 1, 2) + 1;
    a1[8] = a2;
    fn_02382C98(a3, a1 + 9, a2);
    v6 = a1 + a1[7] + 8;
    *v6 = a1[6];
    v6[1] = 1;
    fn_0238E610(a1);
    return v6;
}
