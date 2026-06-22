extern int func_020614d0(void);
extern void func_0206165c(int i, int val);
extern int func_02061558(int a, int b, int c);

int func_020635d8(int a, int b)
{
    int r = func_020614d0();
    if (r != 0) return r;
    if (b != 0 && b != 1) return 6;
    func_0206165c(0x19, a);
    r = func_02061558(0x19, 1, b);
    if (r == 0) return 2;
    return r;
}
