struct A {
    char pad[0x24];
    unsigned int count;
};

struct B {
    char pad[0x1c];
    unsigned int idx;
    char pad2[0x10];
};

void func_ov002_020e6b3c(char *o) {
    void **p = (void **)(((long long)(int)(o + 8)));
    struct A *a = (struct A *)p[0];
    unsigned int n = a->count;
    struct B *b = (struct B *)p[1];
    unsigned int i;
    for (i = 0; i < n; i++, b++)
        b->idx = 0;
}
