int func_02054d88(void)
{
    unsigned int mode = *(volatile unsigned int *)0x4000000 & 7;
    unsigned short h = *(volatile unsigned short *)0x400000e;
    if ((int)mode < 3 || ((int)mode < 6 && !(h & 0x80))) {
        unsigned int v = *(volatile unsigned int *)0x4000000;
        return 0x6000000 + (((v & 0x7000000) >> 0x18) << 0x10) + (((unsigned int)(h & 0x3c) >> 2) << 14);
    }
    return 0;
}
