extern void func_02057410(int a, unsigned int b, int c, unsigned int d);

void func_02057ce8(int a, int b, int c)
{
    unsigned int m;
    m = 0;
    m = ~m;
    m >>= 1;
    func_02057410(a, m, b, (unsigned int)c);
}