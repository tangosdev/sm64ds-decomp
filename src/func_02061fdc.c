extern int func_02061428(int a, int b);
extern void func_0206165c(int i, int val);
extern int func_02061558(int a, int b);

int func_02061fdc(int this)
{
    int r;
    r = func_02061428(1, 2);
    if (r != 0) return r;
    func_0206165c(8, this);
    r = func_02061558(8, 0);
    if (r == 0) return 2;
    return r;
}
