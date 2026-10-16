extern void fn_02388CD8(int);

void fn_0238C8C8(void) {
    volatile unsigned short *status = (volatile unsigned short *)0x040001C0;
    while (*status & 0x80)
        fn_02388CD8(1);
    *status = 0x8100;
    *(volatile unsigned short *)0x040001C2 = 0xAB;
    while (*status & 0x80)
        fn_02388CD8(1);
}
