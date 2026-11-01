/* storage 0x02397AEC runtime 0x027C9A64 linked 0x3C main ARM.
 * Bit 0 of the halfword at base+0x2FE. */

int fn_02397AEC(unsigned int value) {
    unsigned char *base;
    unsigned short *reg;
    if (value > 1)
        return 5;
    base = *(unsigned char **)0x0380FFF4;
    reg = (unsigned short *)(base + 0x2FE);
    *reg = (*reg & ~1) | (value & 1);
    return 0;
}
