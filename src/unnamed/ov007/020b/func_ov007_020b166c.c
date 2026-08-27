extern char *data_ov007_0210342c;
extern void func_ov007_020c1d8c(int i);
extern void func_ov007_020b2764(int n);
extern void RequestScreenFade(int r0, int r1, int r2);
extern void func_ov007_020b16f0(void);

void func_ov007_020b166c(void)
{
    char *g = data_ov007_0210342c;
    int r4 = (*(short *)(*(char **)(g + 4) + 0xa) == 6);
    if (*(int *)(*(char **)(g + 8) + 0xc) != 0)
        return;
    func_ov007_020c1d8c(0);
    func_ov007_020b2764(0x1000);
    if (r4)
        RequestScreenFade(1, 2, (int)func_ov007_020b16f0);
    else
        RequestScreenFade(2, 3, (int)func_ov007_020b16f0);
}
