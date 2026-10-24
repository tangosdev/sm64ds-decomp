/* storage 0x02382A80 runtime 0x037FA918 size 0x30 (listed 0x2C + pool) wram ARM.
 * Wrap into [0, 0x107): add/subtract 0x107. One shared numeric pool (0x107), no calls. */

int fn_02382A80(int a1) {
    while (a1 < 0)
        a1 += 0x107;
    while (a1 >= 0x107)
        a1 -= 0x107;
    return a1;
}
