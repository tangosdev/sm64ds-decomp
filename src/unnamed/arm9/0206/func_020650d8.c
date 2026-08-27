extern void func_02065500(int a, int b, int c);

void func_020650d8(int a, int b, int c)
{
    if (a == 0) return;
    if (b == 0) return;
    if (b == 0xffff) return;
    func_02065500(c, a, b);
}
