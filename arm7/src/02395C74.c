int fn_02395C74(int a1)
{
    volatile unsigned short *p = (volatile unsigned short *)0x480815E;
    while (*p & 1) {
    }
    *(volatile unsigned short *)0x4808158 = a1 | 0x6000;
    while (*p & 1) {
    }
    return *(volatile unsigned short *)0x480815C;
}
