/* storage 0x02383520 runtime 0x037FB3B8 size 0x1C wram ARM. No pool. */

int fn_02383520(int a1) {
    unsigned char *p = (unsigned char *)((a1 << 4) + 0x4000000);
    return (p[0x403] & 0x80) != 0;
}
