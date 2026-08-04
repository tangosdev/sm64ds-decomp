extern void func_ov004_020b2444(int x, int y, int h, int a, int b, int c, int d);

void func_ov006_020fba64(char *base)
{
    int i;
    char *o = base;
    for (i = 0; i < 0x1e; i++) {
        if (*(unsigned short *)(o + 0x4c00 + 0xfa) != 0) {
            func_ov004_020b2444(
                *(int *)(o + 0x4000 + 0xcf0) >> 0xc,
                *(int *)(o + 0x4000 + 0xcf4) >> 0xc,
                *(unsigned short *)(o + 0x4c00 + 0xf8),
                -1, -1, 0, 0);
        }
        o += 0xc;
    }
}
