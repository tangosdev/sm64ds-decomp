void func_0205a61c(void* dst, void* src, int len);

typedef struct {
    char pad0[0x18];
    int f18;
    int f1c;
    int f20;
} G;
extern G data_020a8180;

typedef struct {
    char pad0[8];
    int f8;
} Arg;

int func_02060ebc(Arg* a)
{
    G* g = &data_020a8180;
    int base = g->f18 & ~0x1ff;
    if (base == a->f8) {
        int off = g->f18 - base;
        int len = 0x200 - off;
        if ((unsigned int)len > (unsigned int)g->f20) len = g->f20;
        func_0205a61c((char*)a + 0x20 + off, (void*)g->f1c, len);
        int *p18 = (int*)(((int)g + 0x18) & 0xFFFFFFFFFFFFFFFFULL);
        int *p1c = (int*)(((int)g + 0x1c) & 0xFFFFFFFFFFFFFFFFULL);
        *p18 += len;
        int *p20 = (int*)(((int)g + 0x20) & 0xFFFFFFFFFFFFFFFFULL);
        *p1c += len;
        *p20 -= len;
    }
    return g->f20 != 0;
}
