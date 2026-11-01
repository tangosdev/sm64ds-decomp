/* storage 0x02397AB0 runtime 0x027C9A28 linked 0x3C main ARM.
 * Bit 1 of the halfword at base+0x2FE. Values above 1 are rejected. */

int fn_02397AB0(unsigned int value) {
    unsigned char *base;
    unsigned short *reg;
    if (value > 1)
        return 5;
    base = *(unsigned char **)0x0380FFF4;
    reg = (unsigned short *)(base + 0x2FE);
    *reg = (*reg & ~2) | ((value & 1) << 1);
    return 0;
}
