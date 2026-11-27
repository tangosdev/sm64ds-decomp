#pragma exceptions off

/* storage 0x02399A44 runtime 0x027CB9BC size 0xE0 main ARM.
 * Pools: =0x4808088, =0x380FFF4, =0x48080AE.  Calls fn_023A06F0, fn_023A07BC. */

struct Sub100 {
    unsigned char pad00[0x60];
    unsigned short f60;
    unsigned char pad62[0xA];
    unsigned short f6C;
    unsigned char pad6E[0xA];
    unsigned short f78;
};

struct Sub400 {
    unsigned char pad00[0xEA];
    unsigned short fEA, fEC, fEE;
};

struct Base {
    unsigned char pad00[0x100];
    struct Sub100 s1;
    unsigned char pad180[0x286];
    struct Sub400 s4;
};

extern int fn_023A06F0(void);
extern int fn_023A07BC(int);

int fn_02399A44(void) {
    int result;
    int *v1;

    if (*(unsigned short *)0x4808088 == 0 && ((struct Base *)*(int *)0x380FFF4)->s1.f78 != 0) {
        *(unsigned short *)0x48080AE = 8;
        return fn_023A06F0();
    }
    v1 = (int *)0x380FFF4;
    if ((((struct Base *)*v1)->s4.fEA & ~((struct Base *)*v1)->s4.fEE) == 0) {
        *(unsigned short *)0x48080AE = 8;
        if (((struct Base *)*v1)->s1.f78 != 0)
            fn_023A06F0();
    }
    *(unsigned short *)0x48080AE = 5;
    if (((struct Base *)*(int *)0x380FFF4)->s1.f6C != 0)
        fn_023A07BC(1);
    result = ((struct Base *)*(int *)0x380FFF4)->s1.f60;
    if (result != 0)
        return fn_023A07BC(0);
    return result;
}
