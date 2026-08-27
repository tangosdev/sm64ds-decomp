extern void func_020587e4(void *a, void **out, int c);
extern void _ZN4CP1519InvalidateDataCacheEjj(unsigned int a, unsigned int b);

extern int data_020a5600;

struct Inner {
    char pad8[8];
    int h8;
    char pad30[0x30 - 0xc];
    void (*h30)(unsigned int, unsigned int, unsigned int, int, int);
    int h34;
};

struct Outer {
    struct Inner *p0;
    int h4;
    char pad[4];
    int hc;
    int h10;
};

void func_02050038(void)
{
    struct Outer *r;
    void *out;
    for (;;) {
        func_020587e4(&data_020a5600, &out, 1);
        r = (struct Outer *)out;
        _ZN4CP1519InvalidateDataCacheEjj((unsigned int)r->hc, (unsigned int)r->h4);
        _ZN4CP1519InvalidateDataCacheEjj((unsigned int)r->h10, (unsigned int)r->h4);
        r->p0->h30((unsigned int)r->hc, (unsigned int)r->h10, (unsigned int)r->h4, r->p0->h8, r->p0->h34);
    }
}
