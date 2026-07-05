void func_02016acc(char *a0, unsigned int mask)
{
    char *p = *(char **)(a0 + 8);
    unsigned int n = *(unsigned int *)(p + 0x24);
    char *q = *(char **)(a0 + 0xc);

    for (unsigned int i = 0; i < n; i++) {
        unsigned int nmask = ~mask;
        *(unsigned int *)(((long long)(int)(q + 0x24)) & 0xFFFFFFFFFFFFFFFFLL) &= nmask;
        q += 0x30;
    }
}
