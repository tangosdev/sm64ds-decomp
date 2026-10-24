#pragma exceptions off

/* storage 0x02399498 runtime 0x027CB410 size 0x70 main ARM.
 * Pools: =0x4808010, =0x48080AC, =0x380FFF4, =0x316, =0x4808048.
 * Bit6 flag at +0x316: lsl/lsr test, bic clear. */

struct Cmd {
    unsigned char pad00[0x10];
    unsigned short f10;
    unsigned char pad12[2];
    unsigned short f14;
    unsigned short f16lo:6, flag:1, f16hi:9;
};

int fn_02399498(void) {
    int result;
    int v0;
    struct Cmd *p;
    *(unsigned short *)0x4808010 = 0x2000;
    *(unsigned short *)0x48080AC = 13;
    v0 = *(int *)0x380FFF4;
    p = (struct Cmd *)(v0 + 0x300);
    if (p->flag) {
        result = *(unsigned short *)(v0 + 0x316) & ~0x40;
        *(unsigned short *)(v0 + 0x316) = result;
        return result;
    }
    if (p->f14 != 2)
        return (int)p;
    result = p->f10;
    if (result != 0x40) {
        *(unsigned short *)0x4808048 = 0;
        result = (int)(unsigned short *)0x4808048;
    }
    return result;
}
