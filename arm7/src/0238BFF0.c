/* storage 0x0238BFF0 runtime 0x03803E88 size 0x50 wram. Pool +0x50: 027FFC30, 0000FFFF. */

extern int fn_0238BEE0(void);

int fn_0238BFF0(void) {
    unsigned char *base;
    unsigned int bit;

    base = (unsigned char *)0x027FFC30;
    if (*(unsigned short *)base == 0xFFFF)
        return 0;
    bit = (unsigned int)base[5] << 30;
    bit >>= 31;
    if (bit == 0)
        fn_0238BEE0();
    base = (unsigned char *)0x027FFC30;
    bit = (unsigned int)base[5] << 30;
    bit >>= 31;
    return (int)bit;
}
