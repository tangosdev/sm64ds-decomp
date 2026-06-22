extern void func_02073300(int a, int b, int c);
extern void func_0203cbc0(int a);

void func_02073244(int a, int b, int c, int d)
{
    if (a == 0) return;
    if (d != 0)
        func_02073300(a, *(int*)(a - 4), b);
    func_0203cbc0(a - c);
}
