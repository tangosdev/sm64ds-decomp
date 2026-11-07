extern int dat_038087A0[];
extern int dat_03808DA0;
extern int fn_02387E08(int *);
extern void fn_02387264(void);

int *fn_02387CC8(void)
{
    int *v0 = dat_038087A0;
    int *result = (int *)v0[0xE];
    if (result == 0) {
        v0[0xE] = 1;
        v0[9] = 0;
        v0[8] = v0[9];
        v0[7] = v0[8];
        v0[0xA] = -1;
        v0[0xB] = 0;
        v0[0xC] = 0;
        fn_02387264();
        *(int **)&dat_03808DA0 = (int *)fn_02387E08;
        result = &dat_03808DA0;
    }
    return result;
}
