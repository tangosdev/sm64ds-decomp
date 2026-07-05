extern int func_0205c864(void *s);
extern void func_0205c7e4(void *s);

typedef struct S {
    char pad10[0x10];
    unsigned int f10;
    char pad14[0x20 - 0x14];
    int f20;
    int f24;
    int f28;
    int f2c;
    int f30;
    int f34;
    int f38;
    int f3c;
    int f40;
    int f44;
} S;

int func_0205c91c(S *s)
{
    int r = 0;
    int ret;
    if (((s->f10 & 2) ? 1 : 0) != 0) {
        ret = func_0205c864(s);
        if (((s->f10 & 4) ? 1 : 0) != 0) {
            *(unsigned int *)(((int)&s->f10) & 0xFFFFFFFFFFFFFFFFu) &= ~4u;
            r = s->f38;
            s->f38 = 0;
            s->f20 = s->f30;
            s->f28 = s->f34;
            s->f44 = s->f3c;
        }
        if (ret != 0) func_0205c7e4(s);
    }
    return r;
}
