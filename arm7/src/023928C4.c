/* storage 0x023928C4 runtime 0x027C483C size 0x10C main.
 * Pool +0x10C: 04000006, 000041A0, 000001C7. */

extern int *fn_0238E688(void);

void fn_023928C4(void) {
    int *base;
    int *holder;
    unsigned short *reg;
    int level;
    unsigned int cur;
    int *slot;

    base = fn_0238E688();
    holder = *(int **)((char *)base + 0x1610);
    reg = (unsigned short *)0x04000006;
    level = *reg;
    if (level < 0xD0 || level > 0xD4)
        return;
    cur = (unsigned int)((int *)holder[0])[53];
    if (cur > 0x8C && cur <= 0x12C) {
        *reg = (unsigned short)level;
        slot = (int *)holder[0] + 53;
        *slot = *slot - 0x41;
    } else if (cur > 0x12C && cur <= 0x1F4) {
        *reg = (unsigned short)(level - 1);
        slot = (int *)holder[0] + 53;
        *slot = *slot - 0x82;
    } else if (cur > 0x1F4 && cur <= 0x3E8) {
        *reg = (unsigned short)(level - 2);
        slot = (int *)holder[0] + 53;
        *slot = *slot - 0xC3;
    } else if (cur > 0x3E8 && cur <= 0x41A0) {
        *reg = (unsigned short)(level - 6);
        slot = (int *)holder[0] + 53;
        {
            int delta;

            delta = 0x1C7;
            *slot = *slot - delta;
        }
    }
}
