extern int *func_02061548(void);
extern int func_02061428(int a, int b);
extern void func_0206165c(int i, int val);
extern int func_02061558(int a, int b);

int func_020634fc(int a, int b, int c, int d, unsigned short e)
{
    int *p;
    short *hdr;
    int r;
    p = func_02061548();
    r = func_02061428(1, 2);
    if (r) return r;
    func_0206165c(0x1e, a);
    hdr = (short *)(p[3]);
    hdr[0] = 0x1e;
    hdr[1] = b;
    hdr[2] = c;
    hdr[3] = d;
    hdr[4] = e;
    r = func_02061558(0x1e, 0);
    if (r == 0) return 2;
    return r;
}
