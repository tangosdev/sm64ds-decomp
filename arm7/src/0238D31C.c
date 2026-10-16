/* storage 0x0238D31C runtime 0x038051B4 linked 0x70 wram ARM.
 * Same SPI kick as fn_0238D2AC, with the second halfword equal to 6. */

extern void fn_02388CD8(int arg);

void fn_0238D31C(void) {
    while (*(volatile unsigned short *)0x040001C0 & 0x80)
        fn_02388CD8(1);
    *(volatile unsigned short *)0x040001C0 = 0x8100;
    *(volatile unsigned short *)0x040001C2 = 6;
    while (*(volatile unsigned short *)0x040001C0 & 0x80)
        fn_02388CD8(1);
}
