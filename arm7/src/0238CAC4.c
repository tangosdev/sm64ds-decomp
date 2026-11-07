/* storage 0x0238CAC4 runtime 0x0380495C linked 0x11C wram ARM.
 * SPI write of a 3-byte address: command 0x8900/0xDB, then the three
 * bytes, then 0x8100 and the low byte, then one more wait. */

extern void fn_02388CD8(int arg);

void fn_0238CAC4(unsigned int addr) {
    unsigned short hi = (addr & 0xFF0000) >> 16;
    unsigned short mid = (addr & 0xFF00) >> 8;
    unsigned short lo = addr & 0xFF;

    while (*(volatile unsigned short *)0x040001C0 & 0x80)
        fn_02388CD8(1);
    *(volatile unsigned short *)0x040001C0 = 0x8900;
    *(volatile unsigned short *)0x040001C2 = 0xDB;

    while (*(volatile unsigned short *)0x040001C0 & 0x80)
        fn_02388CD8(1);
    *(volatile unsigned short *)0x040001C2 = (unsigned char)hi;

    while (*(volatile unsigned short *)0x040001C0 & 0x80)
        fn_02388CD8(1);
    *(volatile unsigned short *)0x040001C2 = (unsigned char)mid;

    while (*(volatile unsigned short *)0x040001C0 & 0x80)
        fn_02388CD8(1);
    *(volatile unsigned short *)0x040001C0 = 0x8100;
    *(volatile unsigned short *)0x040001C2 = (unsigned char)lo;

    while (*(volatile unsigned short *)0x040001C0 & 0x80)
        fn_02388CD8(1);
}
