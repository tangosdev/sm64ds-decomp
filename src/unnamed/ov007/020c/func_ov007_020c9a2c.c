#pragma opt_strength_reduction off
void func_ov007_020c9a2c(int *o, int x) {
    int *base;
    int i;

    o[0] = x;
    base = (int *)o[0];
    x += 0x10;
    for (i = 0; i < *(unsigned short *)((char *)base + 0xe); i++) {
        int *slot = o + i;
        int q;
        slot[1] = x;
        q = slot[1];
        slot[0x11] = x + 8;
        x += *(int *)(q + 4);
    }
}
