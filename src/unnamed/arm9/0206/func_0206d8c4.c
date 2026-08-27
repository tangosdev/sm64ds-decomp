extern void _ZN4cstd8__assertEPKcS1_S1_i(const char *, const char *, const char *, int);
extern const char data_020868fc;
extern const char data_02086a08;

struct Elem {
    int a, b, c, d;
};

struct Arr {
    unsigned int count;
    int pad[3];
    struct Elem elems[1];
};

void func_0206d8c4(struct Arr *s, unsigned int index) {
    unsigned int n;
    if (index >= s->count) {
        _ZN4cstd8__assertEPKcS1_S1_i(&data_020868fc, (const char *)0x55, &data_02086a08, 1);
    }
    s->count--;
    n = s->count;
    goto test;
loop:
    s->elems[index] = s->elems[index + 1];
    index++;
test:
    if (index < n) goto loop;
}
