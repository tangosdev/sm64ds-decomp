void *_ZN5Model27LoadCompressedTextureToVramEPcjPc(char *a, unsigned int b, char *c);
int func_02045ad8(void *p);
unsigned int func_02045a50(const void *src, unsigned int size);

int func_ov075_0211aa94(char *c, char *b)
{
    char *d;
    char *a;
    char *e;
    int sz;

    a = *(char **)(b + 0x18);
    d = *(char **)(b + 0x28);
    e = *(char **)(b + 0x20);

    *(int *)(c + 0x88) = (int)_ZN5Model27LoadCompressedTextureToVramEPcjPc(
        *(char **)(a + 4), *(unsigned int *)(a + 8),
        *(char **)(a + 4) + *(unsigned int *)(a + 8));
    sz = *(int *)(e + 8);
    if (sz <= 8)
        *(int *)(c + 0x8c) = func_02045ad8(*(void **)(e + 4));
    else
        *(int *)(c + 0x8c) = (int)func_02045a50(*(const void **)(e + 4), (unsigned int)sz);
    *(int *)(c + 0x90) = *(int *)(d + 0x28);
    *(int *)(c + 0x94) = *(int *)(d + 0x2c);
    *(int *)(c + 0x98) = *(int *)(a + 0x10);
    if (*(int *)(c + 0x88) == 0)
        goto ret0;
    if (*(int *)(c + 0x8c) != 0)
        goto ret1;
ret0:
    return 0;
ret1:
    return 1;
}
