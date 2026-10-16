/* storage 0x0238BE64 runtime 0x03803CFC size 0x30 wram. Pool +0x30: 04000204. */

int fn_0238BE64(volatile int *fields) {
    volatile unsigned short *reg;
    int field0;
    int field1;
    int value;

    field0 = fields[0];
    reg = (volatile unsigned short *)0x04000204;
    value = *reg;
    value = (value & ~0xC) | (field0 << 2);
    *reg = (unsigned short)value;
    field1 = fields[1];
    value = *reg;
    value = (value & ~0x10) | (field1 << 4);
    *reg = (unsigned short)value;
    return value;
}
