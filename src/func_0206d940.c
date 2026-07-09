extern void _ZN4cstd8__assertEPKcPKcPKci(const char *, const char *, const char *, int);
extern const char data_020868fc;
extern const char data_02086a30;

struct Elem {
    int a, b, c, d;
};

struct Arr {
    unsigned int count;
    int pad[3];
    struct Elem elems[1];
};

void func_0206d940(struct Arr *s, unsigned int index) {
    unsigned int n;
    int i;
    if (index > s->count) {
        _ZN4cstd8__assertEPKcPKcPKci(&data_020868fc, (const char *)0x42, &data_02086a30, 1);
    }
    n = s->count;
    if (n - index != 0) {
        i = (int)n - 1;
        goto test;
    loop:
        s->elems[i + 1] = s->elems[i];
        i--;
    test:
        if (i >= (int)index) goto loop;
    }
    s->count++;
}
