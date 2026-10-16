/* storage 0x0239D584 runtime 0x027CF4FC linked 0x68 main ARM.
 * Command 1. If the mode is at most 0x20 and bit 2 at +0x316 is clear,
 * either return 14 or call fn_023869F4(0x10). */

extern int fn_023943D8(int arg);
extern void fn_023869F4(int arg);

int fn_0239D584(int unused, unsigned short *out) {
    unsigned char *base;
    unsigned short bit;
    int shifted;
    out[1] = 1;
    base = *(unsigned char **)0x0380FFF4;
    base += 0x300;
    if (*(unsigned short *)(base + 0x10) > 0x20)
        return 1;
    bit = *(unsigned short *)(base + 0x16);
    shifted = bit << 29;
    if ((unsigned int)shifted >> 31)
        return 1;
    if (fn_023943D8(0) != 0)
        return 14;
    fn_023869F4(0x10);
    return 0;
}
