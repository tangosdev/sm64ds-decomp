void fn_02387F00(unsigned int a1, unsigned int a2)
{
    while (*(volatile int *)0x40001A4 & 0x80000000)
        ;
    *(volatile unsigned char *)0x40001A1 = 0xC0;
    *(volatile unsigned char *)0x40001A8 = a1 >> 24;
    *(volatile unsigned char *)0x40001A9 = a1 >> 16;
    *(volatile unsigned char *)0x40001AA = a1 >> 8;
    *(volatile unsigned char *)0x40001AB = a1;
    *(volatile unsigned char *)0x40001AC = a2 >> 24;
    *(volatile unsigned char *)0x40001AD = a2 >> 16;
    *(volatile unsigned char *)0x40001AE = a2 >> 8;
    *(volatile unsigned char *)0x40001AF = a2;
}
