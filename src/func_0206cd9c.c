#pragma optimize_for_size on

extern int func_0206cf7c(int mask);

struct S {
    unsigned short f0;
    unsigned short f2;
    unsigned short f4;
};

void func_0206cd9c(struct S *s)
{
    unsigned short v;

    if (func_0206cf7c(1) == 0) {
        return;
    }
    if (s->f4 == 0) {
        return;
    }
    *(volatile unsigned short *)0x27fff72 = s->f2;
    *(volatile unsigned short *)0x9fe2ffe = s->f2;
    v = *(volatile unsigned short *)0x9fe20f0;
    v = *(volatile unsigned short *)0x9fe20f0;
    v = *(volatile unsigned short *)0x9fe20f0;
    *(volatile unsigned short *)0x4000204 = s->f0;
}