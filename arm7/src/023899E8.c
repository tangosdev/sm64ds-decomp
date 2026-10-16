/* storage 0x023899E8 runtime 0x03801880 size 0x14 + 8 pool wram ARM.
 * Two constant pools (0x00008002, 0x040001C0). Returns the reg address. */

volatile unsigned short *fn_023899E8(int a1) {
    volatile unsigned short *r0 = (volatile unsigned short *)0x40001C0;
    *r0 = 0x8002 | (a1 << 11);
    return r0;
}
