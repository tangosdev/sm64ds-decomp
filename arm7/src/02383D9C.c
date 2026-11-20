extern int fn_0238D3E4(long long, long long);

int fn_02383D9C(int *a1, int a2)
{
    int v4 = *(short *)((char *)a1 + 0x32);
    int v6, v7, result;
    if (v4 == 0)
        return 0;
    v6 = a1[5];
    v7 = a1[6];
    if (v6 >= v7)
        return 0;
    result = fn_0238D3E4((long long)v4 * (v7 - v6), (long long)v7);
    if (a2 != 0)
        a1[5]++;
    return result;
}
