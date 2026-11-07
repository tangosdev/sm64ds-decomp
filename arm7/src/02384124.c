/* storage 0x02384124 runtime 0x037FBFBC size 0x24 wram ARM. No pool, no calls. */

int fn_02384124(unsigned char *a1) {
    a1[0] = 0;
    a1[2] = 0;
    a1[3] = 1;
    a1[1] = 0x10;
    *(short *)(a1 + 4) = 0;
    return (int)a1;
}
