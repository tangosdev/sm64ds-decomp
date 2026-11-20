/* storage 0x02397D60 runtime 0x027C9CD8 linked 0x44 main ARM.
 * Reject a zero low halfword, store it at base+0x2F0, set bit 2 at +0x304. */

int fn_02397D60(int value) {
    if ((value & 0x7FFE) == 0)
        return 5;
    *(unsigned short *)(*(unsigned char **)0x0380FFF4 + 0x2F0) = value;
    *(unsigned int *)(*(unsigned char **)0x0380FFF4 + 0x304) |= 4;
    return 0;
}
