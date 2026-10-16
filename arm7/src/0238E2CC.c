#pragma scheduling off
#pragma opt_common_subs off
extern void *fn_0238E688(void);
extern unsigned short *fn_0238E57C(unsigned short *a, unsigned short *b);
extern int fn_02382C80(int a1, int a2, int a3);
extern int fn_02382DC0(int a1, int a2, int a3);
extern void *fn_0238FE18(unsigned short *a2);
extern int fn_0238E088(int a1, int a2, int a3);

int fn_0238E2CC(int a1, unsigned short *a2) {
    int *rec;
    unsigned short v;
    volatile unsigned short z0, z1, z2;
    rec = *(int **)((char *)fn_0238E688() + 0x1610);
    fn_0238E57C(a2 + 8, (unsigned short *)(*rec + 0xDC));
    a2[11] = 7;
    a2[12] = *(unsigned short *)((char *)*rec + 0x1F0);
    a2[15] = *(unsigned short *)((char *)*rec + 0x1E8);
    a2[14] = *(unsigned short *)((char *)*rec + 0xE2);
    if (*(int *)((char *)*rec + 0x194) == 0) {
        a2[16] = 0;
        a2[17] = 0;
        z0 = 0;
        fn_02382C80(z0, (int)(a2 + 18), 0x50);
        a2[79] = 0;
    } else {
        fn_0238E688();
        a2[16] = *(unsigned short *)((char *)*rec + 0x192);
        a2[17] = 0;
        fn_02382DC0(*rec + 0x198, (int)(a2 + 18), 0x50);
        a2[79] = 1;
    }
    a2[58] = 1;
    a2[59] = 1;
    if (*(unsigned short *)((char *)*rec + 0xE2) == 1)
        a2[60] = 0;
    else
        a2[60] = 16;
    a2[61] = 10;
    z1 = 0;
    fn_02382C80(z1, (int)(a2 + 62), 8);
    z2 = (unsigned short)0xFFFF;
    fn_02382C80(z2, (int)(a2 + 66), 0x18);
    a2[78] = *(unsigned short *)((char *)*rec + 0x1EA);
    v = *(unsigned short *)((char *)fn_0238FE18(a2) + 4);
    if (v == 0)
        return 1;
    fn_0238E088(a1, 0x200, v);
    return 0;
}
