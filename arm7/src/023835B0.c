#pragma exceptions off

/* storage 0x023835B0 runtime 0x037FB448 size 0x78 wram ARM.
 * Pools: =unk_38070B8, =dat_38070B4, =0xFFF5. Calls fn_02383450. */

extern int dat_38070B4;
extern unsigned char unk_38070B8[];
extern int fn_02383450(int, int);

unsigned short *fn_023835B0(int a1, int a2, int a3) {
    unsigned short *v2;
    unsigned char *p;
    unsigned char b;
    unk_38070B8[a1] = a2;
    if (dat_38070B4 > 0 && ((1 << a1) & 0xFFF5)) {
        p = (unsigned char *)((a1 << 4) + 0x4000000);
        b = ((volatile unsigned char *)p)[0x402];
        a2 = fn_02383450(a2, b);
    }
    a2 |= a3 << 8;
    v2 = (unsigned short *)((a1 << 4) + 0x4000000);
    v2[0x200] = a2;
    return v2 + 0x200;
}
