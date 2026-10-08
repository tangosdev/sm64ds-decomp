#pragma optimize_for_size on
#pragma opt_common_subs off

extern int func_0206cf7c(int mask);

struct S {
    unsigned short f0;
    unsigned short f2;
    unsigned short f4;
};

void func_0206cd9c(struct S *s)
{
    volatile unsigned short scratch;

    if (func_0206cf7c(1) != 0 && s->f4 != 0) {
        volatile unsigned short *p27;
        volatile unsigned short *p9fe;
        unsigned short v2;

        p27 = (volatile unsigned short *)0x27fff72;
        p9fe = (volatile unsigned short *)0x9fe2ffe;
        v2 = s->f2;
        *p27 = v2;
        v2 = s->f2;
        *p9fe = v2;
        scratch = *(volatile unsigned short *)0x9fe20f0;
        scratch = *(volatile unsigned short *)0x9fe20f0;
        scratch = *(volatile unsigned short *)0x9fe20f0;
    }
    *(volatile unsigned short *)0x4000204 = s->f0;
}