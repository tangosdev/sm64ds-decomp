#pragma exceptions off

/* storage 0x023845D4 runtime 0x037FC46C size 0x50 wram ARM.
 * Pool: =0x1F46 numeric. Calls fn_02383F7C. */

extern void fn_02383F7C(unsigned char *, int);

int fn_023845D4(unsigned char *a1, int a2, int a3) {
    unsigned int v3 = *a1;
    if (v3 < 8)
        return 0;
    if (v3 > 0xD)
        return 0;
    a1[1] = 1;
    *(int *)(a1 + 0x44) = a2;
    *(short *)(a1 + 0x3C) = 0x1F46;
    fn_02383F7C(a1, a3);
    return 1;
}
