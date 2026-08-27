void MultiStore_Int(int val, int *dst, int len);
void _ZN4CP1527FlushAndInvalidateDataCacheEjj(unsigned int a, unsigned int b);
void func_0205ac28(void *a, int b, int c, int d);

typedef struct {
    int f0;
    int pad4[2];
    int* dstc;
    int* dst10;
    int len;
    int f18;
    int pad1c;
    void* f20;
    int f24;
    int f28;
} G_t;
extern G_t data_020a5634;

void func_020501b8(void)
{
    G_t* g = &data_020a5634;
    volatile int a;
    volatile int b;
    if (g->f0 == 0) return;
    g->f18 = 0;
    a = 0;
    MultiStore_Int(a, g->dstc, g->len);
    b = 0;
    MultiStore_Int(b, g->dst10, g->len);
    _ZN4CP1527FlushAndInvalidateDataCacheEjj((unsigned int)g->dstc, g->len);
    _ZN4CP1527FlushAndInvalidateDataCacheEjj((unsigned int)g->dst10, g->len);
    {
        int sh = g->f28;
        int mask = (sh >= 0) ? (1 << sh) : 0;
        func_0205ac28(g->f20, g->f24, mask, 0);
    }
}
