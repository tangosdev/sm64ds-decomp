extern void func_0201361c(int idx);
extern unsigned int data_0209caa0[];

void func_02013868(int a, int b)
{
    int idx = (a << 2) | b;
    data_0209caa0[3] |= 1 << idx;
    func_0201361c(idx);
}
