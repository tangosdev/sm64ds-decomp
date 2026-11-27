#pragma exceptions off

/* storage 0x02399B30 runtime 0x027CBAA8 size 0xEC main ARM.
 * Pools: =0x380FFF4, =0x558.  Calls fn_02399C24, fn_02396CF0, fn_0239AE84. */

struct Sub300 {
    unsigned char pad00[0xA2];
    unsigned short fA2;
};

struct Sub3E4 {
    unsigned char pad00[0x3C];
    unsigned short f3C;
    unsigned char pad3E[2];
    unsigned short f40;
    unsigned char pad42[2];
    int f44;
    unsigned char pad48[0x48];
    int f90;
    unsigned char pad94[0xC];
    unsigned short fA0;
};

struct Base {
    unsigned char pad00[0x300];
    struct Sub300 s3;
    unsigned char pad3A4[0x40];
    struct Sub3E4 sE;
};

extern void fn_02399C24(void);
extern void fn_02396CF0(void);
extern void fn_0239AE84(int, int);

void fn_02399B30(void) {
    struct Sub3E4 *v0;
    int *v1;
    unsigned short v2;

    v0 = &((struct Base *)*(int *)0x380FFF4)->sE;
    if (*(unsigned short *)(v0->f90 + 0x10) != *(unsigned short *)(v0->f44 + 2))
        fn_02399C24();
    v2 = *(unsigned short *)(v0->f44 + 4) & 0xFF;
    if (v2 != 0)
        *(int *)(*(int *)0x380FFF4 + 0x558) += v2;
    else
        ++*(int *)(*(int *)0x380FFF4 + 0x558);
    ++v0->f40;
    *(unsigned short *)(v0->f90 + 0x12) =
        *(unsigned short *)(v0->f90 + 0x10) ^ *(unsigned short *)(v0->f44 + 2);
    v0->fA0 = *(unsigned short *)(v0->f90 + 0x10);
    v0->f3C = 0;
    v1 = (int *)0x380FFF4;
    if (((struct Base *)*v1)->s3.fA2 != 0)
        fn_02396CF0();
    fn_0239AE84(*(int *)0x380FFF4 + 0x14C, v0->f90);
}
