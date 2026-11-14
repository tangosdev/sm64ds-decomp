/* storage 0x0238D2AC runtime 0x03805144 linked 0x70 wram ARM.
 * Wait until SPI bit 7 clears, start a transfer with 0x8100 / 4, wait again. */

extern void fn_02388CD8(int arg);

void fn_0238D2AC(void) {
    while (*(volatile unsigned short *)0x040001C0 & 0x80)
        fn_02388CD8(1);
    *(volatile unsigned short *)0x040001C0 = 0x8100;
    *(volatile unsigned short *)0x040001C2 = 4;
    while (*(volatile unsigned short *)0x040001C0 & 0x80)
        fn_02388CD8(1);
}
