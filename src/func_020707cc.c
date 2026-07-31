double func_020707cc(double x)
{
    int a = (int)((long long)(int)&x);
    int *p = (int *)((long long)(a + 4));
    *p &= 0x7fffffff;
    return x;
}
