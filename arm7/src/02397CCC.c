/* storage 0x02397CCC runtime 0x027C9C44 linked 0x94 main ARM.
 * Store a mode 0..3, fold it into 0x04808006, and set bit 3 at +0x304. */

extern void fn_02386F44(int arg);

int fn_02397CCC(unsigned short value) {
    unsigned short bit;
    int shifted;
    if (value > 3)
        return 5;
    *(unsigned short *)(*(unsigned char **)0x0380FFF4 + 0x2F2) = value;
    *(unsigned short *)(*(unsigned char **)0x0380FFF4 + 0x314) = value;
    *(unsigned short *)0x04808006 = (*(unsigned short *)0x04808006 & 0xFFF8) | value;
    bit = *(unsigned short *)(*(unsigned char **)0x0380FFF4 + 0x316);
    shifted = bit << 31;
    fn_02386F44((unsigned int)shifted >> 31);
    *(unsigned int *)(*(unsigned char **)0x0380FFF4 + 0x304) |= 8;
    return 0;
}
