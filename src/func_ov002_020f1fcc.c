// NONMATCHING: different op / idiom (div=22). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern void _ZN3G2x13SetBlendAlphaEPVttttt(
    volatile void *reg,
    unsigned short a, unsigned short b, int c, int d);

extern unsigned char BOTTOM_SCREEN_RELATED;

void func_ov002_020f1fcc(unsigned char *self)
{
    unsigned char *s = *(unsigned char **)(self + 0xd4);
    int i, v;
    if (*(unsigned short *)(s + 0x2e) == 0)
        return;
    (*(unsigned short *)(s + 0x2c))++;
    s = *(unsigned char **)(self + 0xd4);
    if (*(unsigned short *)(s + 0x2c) != 0x18)
        return;
    *(unsigned short *)(s + 0x2c) = 0;
    s = *(unsigned char **)(self + 0xd4);
    (*(unsigned short *)(s + 0x2e))--;
    s = *(unsigned char **)(self + 0xd4);
    v = *(unsigned short *)(s + 0x2e);
    if (v != 0) {
        _ZN3G2x13SetBlendAlphaEPVttttt(
            (volatile void *)0x4001050, 4, 0x28, v, 0x10 - v);
        return;
    }
    BOTTOM_SCREEN_RELATED &= ~4;
    *(volatile unsigned int *)0x4001000 =
        (*(volatile unsigned int *)0x4001000 & ~0x1f00) | (BOTTOM_SCREEN_RELATED << 8);
    _ZN3G2x13SetBlendAlphaEPVttttt(
        (volatile void *)0x4001050, 0, 0x28, 0xc, 4);
    s = *(unsigned char **)(self + 0xd4);
    (*(unsigned char *)(s + 0x34))++;
    for (i = 0; i < 3; i++) {
        s = *(unsigned char **)(self + 0xd4);
        *(unsigned short *)(s + i * 2 + 0x24) = (i + 1) << 4;
    }
}
