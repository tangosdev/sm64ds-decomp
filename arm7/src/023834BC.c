#pragma exceptions off

/* storage 0x023834BC runtime 0x037FB354 size 0x58 wram ARM.
 * Pools: =dat_38070B4, =unk_38070B8, =0xFFF5. Calls fn_02383450. */

extern int dat_38070B4;
extern unsigned char unk_38070B8[];
extern int fn_02383450(int, int);

void fn_023834BC(int a1) {
    int i;
    dat_38070B4 = a1;
    for (i = 0; i < 16; i++)
        if ((1 << i) & 0xFFF5) {
            int v = i << 4;
            unsigned char b = ((volatile unsigned char *)(v + 0x4000000))[0x402];
            ((unsigned char *)(v + 0x4000000))[0x400] =
                fn_02383450(unk_38070B8[i], b);
        }
}
