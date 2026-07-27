double func_020707cc(double x)
{
    int a = (int)((long long)(int)&x & 0xffffffffffffffffLL);
    int *p = (int *)((long long)(a + 4) & 0xffffffffffffffffLL);
    *p &= 0x7fffffff;
    return x;
}
