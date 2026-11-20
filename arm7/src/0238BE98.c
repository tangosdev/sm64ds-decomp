/* storage 0x0238BE98 runtime 0x03803D30 size 0x44 wram. Pool +0x44: 04000204. */

int fn_0238BE98(int *fields) {
    volatile unsigned short *reg;
    int value;

    reg = (volatile unsigned short *)0x04000204;
    fields[0] = (*reg & 0xC) >> 2;
    fields[1] = (*reg & 0x10) >> 4;
    value = *reg;
    value = (value & ~0xC) | 0xC;
    *reg = (unsigned short)value;
    value = *reg;
    value = value & ~0x10;
    *reg = (unsigned short)value;
    return value;
}
