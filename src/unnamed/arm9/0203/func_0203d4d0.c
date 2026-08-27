int func_0203d4d0(void *p, int *a, int *b)
{
    int t1 = (int)(((long long)a[0] * b[1] + 0x800) >> 12);
    int t2 = (int)(((long long)a[1] * b[0] + 0x800) >> 12);
    return t1 - t2;
}
