extern void *data_0209ee74;

void func_02021cd4(char *self)
{
    int idx = *(int *)(self + 4);
    unsigned short val = *(unsigned short *)(self + 8);

    char *a = *(char **)((char *)data_0209ee74 + 4);
    char *b = *(char **)(a + 0x1c);
    char *elem = *(char **)(b + idx * 32);
    *(unsigned short *)(elem + 0x28) = val;

    char *field = *(char **)(self + 0xc);
    int *flagAddr = (int *)(((long long)(int)(field + 0x1c)) & 0xFFFFFFFFFFFFFFFFLL);
    int v = *flagAddr;
    v &= ~1;
    v |= 1;
    *flagAddr = v;

    *(int *)self = 0;
}
