#pragma exceptions off

/* storage 0x02384588 runtime 0x037FC420 size 0x48 wram ARM.
 * Pool: =0x1F46 numeric. Calls fn_02383F7C. */

extern void fn_02383F7C(unsigned char *, int);

int fn_02384588(unsigned char *a1, int a2) {
    unsigned int v2 = *a1;
    if (v2 < 0xE)
        return 0;
    if (v2 > 0xF)
        return 0;
    a1[1] = 2;
    *(short *)(a1 + 0x3C) = 0x1F46;
    fn_02383F7C(a1, a2);
    return 1;
}
