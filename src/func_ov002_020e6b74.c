void func_ov002_020e6b74(char *c, int *src)
{
    char *base = (char *)(((long long)(int)(c + 8)) & 0xFFFFFFFFFFFFFFFFLL);
    void *obj = *(void **)base;
    unsigned int n = *(unsigned int *)((char *)obj + 0x24);
    char *dst = *(char **)(base + 4);
    unsigned int i;
    for (i = 0; i < n; i++) {
        *(int *)(dst + 0x1c) = *src;
        dst += 0x30;
        src += 1;
    }
}
