/* storage 0x0238B8C0 runtime 0x03803758 size 0x40 wram ARM.
 * Clear the state block: the dword at +0x20 and sixteen halfwords.
 * The stored zero is a copy of the index taken before the loop, and the
 * halfwords are written through the global so the base stays in r0.
 * One ABS32 pool (word_380986C) and one numeric pool (0x0400010E). */

extern short word_380986C[];

int fn_0238B8C0(void) {
    int i;
    int zero;
    volatile unsigned short *reg;
    int result;

    i = 0;
    ((int *)word_380986C)[8] = i;
    zero = i;
    do {
        word_380986C[i] = (short)zero;
        i = i + 1;
    } while (i < 16);
    reg = (volatile unsigned short *)0x400010E;
    result = *reg & ~0x80;
    *reg = (unsigned short)result;
    return result;
}
