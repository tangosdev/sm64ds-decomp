//cpp
struct SharedFilePtr { int w[2]; };
extern "C" void *func_0201787c(SharedFilePtr *sfp);
extern "C" int func_02045ad8(void *p);
extern "C" unsigned int func_02045a50(const void *src, unsigned int size);
extern "C" int _ZN5Model27LoadCompressedTextureToVramEPcjPc(char *, unsigned int, char *);
extern "C" void _ZN13SharedFilePtr7ReleaseEv(SharedFilePtr *self);
extern unsigned char data_ov080_0212869c[];
extern unsigned char data_ov080_02128688[];
extern SharedFilePtr *data_ov080_0212775c[];
extern "C" unsigned char *func_ov080_02125630(void *unused, int i)
{
    int off = 0x18 * i;
    unsigned char *e = &data_ov080_02128688[off];
    unsigned char *fl = &data_ov080_0212869c[off];
    if (data_ov080_0212869c[off] == 0) {
        char *a;
        char *b;
        char *c;
        SharedFilePtr *sfp = data_ov080_0212775c[i];
        char *o = (char *)func_0201787c(sfp);
        a = *(char **)(o + 0x18);
        b = *(char **)(o + 0x28);
        c = *(char **)(o + 0x20);
        int sz;
        *(int *)(e + 0) = _ZN5Model27LoadCompressedTextureToVramEPcjPc(
            *(char **)(a + 4), *(unsigned int *)(a + 8),
            *(char **)(a + 4) + *(unsigned int *)(a + 8));
        sz = *(int *)(c + 8);
        if (sz <= 8)
            *(int *)(e + 4) = func_02045ad8(*(void **)(c + 4));
        else
            *(int *)(e + 4) = (int)func_02045a50(*(const void **)(c + 4), (unsigned int)sz);
        *(int *)(e + 8) = *(int *)(b + 0x28);
        *(int *)(e + 0xc) = *(int *)(b + 0x2c);
        *(int *)(e + 0x10) = *(int *)(a + 0x10);
        *fl = 1;
        _ZN13SharedFilePtr7ReleaseEv(sfp);
    }
    return e;
}
