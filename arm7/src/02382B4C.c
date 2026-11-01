/* storage 0x02382B4C runtime 0x037FA9E4 size 0x30 wram ARM. No pool, one call. */

extern int fn_02383024(int a1, int a2, int a3);

int fn_02382B4C(int a1) {
    int v = a1 << 8;
    int r;
    do {
        r = fn_02383024(12, v, 0);
    } while (r != 0);
    return r;
}
