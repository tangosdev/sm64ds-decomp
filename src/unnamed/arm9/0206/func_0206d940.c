extern void _ZN4cstd8__assertEPKcS1_S1_i(const char *, const char *, const char *, int);
#pragma optimize_for_size on /* built for size like func_0206d32c: the loop enters at its test */
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
        _ZN4cstd8__assertEPKcS1_S1_i(&data_020868fc, (const char *)0x42, &data_02086a30, 1);
    }
    n = s->count;
    if (n - index != 0) {
        for (i = (int)n - 1; i >= (int)index; i--)
            s->elems[i + 1] = s->elems[i];
    }
    s->count++;
}
