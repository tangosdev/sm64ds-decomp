extern int func_ov004_020ad674(void);
extern void func_ov004_020b1de8(int a, int b, int c, int d);
extern void func_ov004_020af948(void *a, int b, int c, int d);
extern void func_ov004_020b2444(int a, int b, int c, int d, int e, int f, int g);
extern unsigned char *data_ov004_020bc83c[];

void func_ov004_020b653c(int arg)
{
    int absv;
    int r4;
    int r6;

    if (arg < 0) absv = -arg; else absv = arg;

    if (arg >= 0) {
        int idx = func_ov004_020ad674();
        r4 = 0x14;
        r6 = *(int *)(data_ov004_020bc83c[idx] + 0x34);
    } else {
        int idx = func_ov004_020ad674();
        r4 = 0x28;
        r6 = *(int *)(data_ov004_020bc83c[idx] + 0x38);
    }

    func_ov004_020b1de8(0x6e, 0x60, 0, -1);
    func_ov004_020af948((void *)r6, 0x80, 0x60, 0);
    func_ov004_020b2444(0x92, 0x60, absv, 0, -1, 2, r4);
}
