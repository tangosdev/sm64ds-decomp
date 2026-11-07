#pragma exceptions off

/* storage 0x0238353C runtime 0x037FB3D4 size 0x50 wram ARM.
 * Pools: =dat_38070B4, =0xFFF5, =unk_38070B8. Calls fn_02383450. */

extern int dat_38070B4;
extern unsigned char unk_38070B8[];
extern int fn_02383450(int, int);

void fn_0238353C(int a1, char a2) {
    int v2 = a1 << 4;
    ((unsigned char *)(v2 + 0x4000000))[0x402] = a2;
    if (dat_38070B4 > 0 && ((1 << a1) & 0xFFF5))
        ((unsigned char *)(v2 + 0x4000000))[0x400] =
            fn_02383450(unk_38070B8[a1], a2);
}
