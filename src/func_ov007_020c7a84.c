void func_ov007_020c7a84(char *p, int f1, int f2)
{
    char *a = *(char **)p;
    int t = *(int *)(*(char **)a + 8) - *(int *)(*(char **)*(char **)(p + 4) + 8) - *(int *)(p + 0x14);
    int m1 = (int)(((long long)f1 * t + 0x800) >> 12);
    int m2 = (int)(((long long)f2 * *(int *)(a + 0xc) + 0x800) >> 12);
    int nm1 = -m1;
    int *pa18 = (int *)(int)(((long long)(int)(a + 0x18)) & 0xFFFFFFFFFFFFFFFFLL);
    *pa18 = *pa18 + (nm1 - m2);
    {
        char *b = *(char **)(p + 4);
        int m3 = (int)(((long long)f2 * *(int *)(b + 0xc) + 0x800) >> 12);
        int *pb18 = (int *)(int)(((long long)(int)(b + 0x18)) & 0xFFFFFFFFFFFFFFFFLL);
        *pb18 = *pb18 - (nm1 + m3);
    }
}
