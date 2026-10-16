extern int dat_03808DA0[];
extern int dat_038087A0[];
extern int dat_03806E60[];
extern int fn_02387E08(int *);
extern void fn_02382C08(unsigned int);

int fn_02387D38(unsigned int a1, int a2, int a3, int a4)
{
    int *v4 = dat_03808DA0;
    dat_038087A0[0xA] = a1;
    dat_038087A0[7] = a2;
    dat_038087A0[8] = a3;
    dat_038087A0[9] = a4;
    dat_038087A0[0xB] = 0;
    dat_038087A0[0xC] = 0;
    if (a1 <= 3)
        fn_02382C08(a1);
    v4[1] = (*(int *)0x27FFE60 & ~0x07000000) | 0xA1000000;
    dat_038087A0[0x2E] = dat_03806E60[2];
    return fn_02387E08(v4);
}
