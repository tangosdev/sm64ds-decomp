double func_020707a4(double x, double y)
{
    *(1 + (int *)&x) = (*(1 + (int *)&x) & 0x7fffffff) | (*(1 + (int *)&y) & 0x80000000);
    return x;
}
