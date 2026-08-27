int func_02052fdc(void) {
    while (*(volatile unsigned short *)0x40002b0 & 0x8000)
        ;
    return (*(volatile unsigned int *)0x40002b4 + 0x200) >> 10;
}
