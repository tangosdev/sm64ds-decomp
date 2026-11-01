extern void fn_0238E610(unsigned short *);
extern void fn_02382C98(void *, void *, unsigned int);

unsigned short *fn_0238FF7C(unsigned short *a1, short *a2)
{
    unsigned short *v4;
    a1[0] = 0;
    a1[1] = 0;
    a1[2] = 0;
    a1[3] = 0;
    a1[4] = 0;
    a1[5] = 0;
    a1[6] = 0x100;
    a1[7] = 0x18;
    fn_02382C98(a2, a1 + 8, 0x30);
    a2[1] = 0;
    a2[2] = 0;
    a2[4] = 0;
    a2[5] = 0;
    a2[6] = 0;
    a2[8] = 0;
    a2[9] = 0;
    a2[10] = 0;
    a2[11] = 0;
    a2[18] = 0;
    a2[19] = 0;
    a2[20] = 0;
    a2[21] = 0;
    v4 = a1 + a1[7] + 8;
    *v4 = a1[6];
    v4[1] = 2;
    fn_0238E610(a1);
    return v4;
}
