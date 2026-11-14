/* storage 0x02397BB0 runtime 0x027C9B28 linked 0xE0 main ARM.
 * Mode 0..3 at base+0x2F8. Nonzero sets bits at +0x378 and +0x386;
 * zero clears them. Then a channel mirror, then 0x04808006 bits 3..5. */

int fn_02397BB0(unsigned int value) {
    unsigned char *base;
    unsigned short *reg;
    if (value > 3)
        return 5;
    *(unsigned short *)(*(unsigned char **)0x0380FFF4 + 0x2F8) = value;
    if (value == 0) {
        reg = (unsigned short *)(*(unsigned char **)0x0380FFF4 + 0x378);
        *reg &= ~0x10;
        reg = (unsigned short *)(*(unsigned char **)0x0380FFF4 + 0x386);
        *reg &= ~0x4000;
    } else {
        reg = (unsigned short *)(*(unsigned char **)0x0380FFF4 + 0x378);
        *reg |= 0x10;
        reg = (unsigned short *)(*(unsigned char **)0x0380FFF4 + 0x386);
        *reg |= 0x4000;
    }
    base = *(unsigned char **)0x0380FFF4;
    if (*(unsigned short *)(base + 0x310) == 64 && *(unsigned short *)(base + 0x2F2) == 1) {
        unsigned short mirrored = *(unsigned short *)(base + 0x378);
        *(unsigned short *)(*(unsigned int *)(base + 0x464) + 0x2E) = mirrored;
    }
    if (value == 0)
        value = 1;
    *(unsigned short *)0x04808006 =
        (*(unsigned short *)0x04808006 & 0xFFC7) | (value << 3);
    return 0;
}
