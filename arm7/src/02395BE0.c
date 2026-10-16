int fn_02395BE0(int a1, int a2)
{
    unsigned int v2;
    volatile unsigned short *p = (volatile unsigned short *)0x480815E;
    v2 = 0;
    while (*p & 1) {
        if (v2++ > 0x2710)
            return -1;
    }
    *(volatile unsigned short *)0x480815A = a2;
    *(volatile unsigned short *)0x4808158 = a1 | 0x5000;
    v2 = 0;
    while (*p & 1) {
        if (v2++ > 0x2710)
            return -1;
    }
    return 0;
}
