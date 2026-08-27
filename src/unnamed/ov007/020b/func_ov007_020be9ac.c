extern void func_ov007_020bc02c(void *p, int c);
extern void func_ov007_020beb80(void *a);

struct A {
    unsigned char b0;
    unsigned char b1;
    char pad2[2];
    char *f4;        /* 0x4 */
    char pad8[0x10];
    void (*fp)(struct A *); /* 0x18 */
};

void func_ov007_020be9ac(struct A *a, char *b, int c) {
    char *p = *(char **)(b + 0x18);
    switch (*(short *)(p + 0xc)) {
    case 0:
        break;
    case 1:
    case 2:
    {
        char *pp = a->f4;
        if (*(int *)(pp + 0x94) != 0) *(int *)(pp + 0x94) = 0;
        break;
    }
    }

    {
        char *q = a->f4;
        if (*(int *)(q + 0x94) != 0 && a->b0 == 1 && a->b1 == 0) {
            *(int *)(q + 0x94) = 0;
        }
    }

    func_ov007_020bc02c(a->f4, c);
    if (a->fp != 0) a->fp(a);
    func_ov007_020beb80(a);
}
