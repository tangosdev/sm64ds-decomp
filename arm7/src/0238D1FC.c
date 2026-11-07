/* storage 0x0238D1FC runtime 0x03805094 linked 0xB0 wram ARM.
 * SPI command 0x8900/5, then 0x8100/0, then the response byte. */

extern void fn_02388CD8(int arg);

unsigned char fn_0238D1FC(unsigned char *out) {
    while (*(volatile unsigned short *)0x040001C0 & 0x80)
        fn_02388CD8(1);
    *(volatile unsigned short *)0x040001C0 = 0x8900;
    *(volatile unsigned short *)0x040001C2 = 5;
    while (*(volatile unsigned short *)0x040001C0 & 0x80)
        fn_02388CD8(1);
    *(volatile unsigned short *)0x040001C0 = 0x8100;
    *(volatile unsigned short *)0x040001C2 = 0;
    while (*(volatile unsigned short *)0x040001C0 & 0x80)
        fn_02388CD8(1);
    unsigned char value = *(volatile unsigned short *)0x040001C2;
    *out = value;
    return value;
}
