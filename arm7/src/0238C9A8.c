extern void fn_02388CD8(int);

int fn_0238C9A8(unsigned int a1) {
    unsigned short hi = (unsigned short)((a1 & 0xFF0000) >> 16);
    unsigned int mid = ((a1 & 0xFF00) << 8) >> 16;
    unsigned short lo = (unsigned short)(a1 & 0xFF);
    volatile unsigned short *st;
    volatile unsigned short *data;
    st = (volatile unsigned short *)0x040001C0;
    while (*st & 0x80)
        fn_02388CD8(1);
    *st = 0x8900;
    data = (volatile unsigned short *)0x040001C2;
    *data = 0xD8;
    st = (volatile unsigned short *)0x040001C0;
    while (*st & 0x80)
        fn_02388CD8(1);
    data = (volatile unsigned short *)0x040001C2;
    *data = (unsigned char)hi;
    st = (volatile unsigned short *)0x040001C0;
    while (*st & 0x80)
        fn_02388CD8(1);
    data = (volatile unsigned short *)0x040001C2;
    *data = (unsigned char)mid;
    st = (volatile unsigned short *)0x040001C0;
    while (*st & 0x80)
        fn_02388CD8(1);
    *st = 0x8100;
    data = (volatile unsigned short *)0x040001C2;
    *data = (unsigned char)lo;
    int bit;
    st = (volatile unsigned short *)0x040001C0;
    while ((bit = *st & 0x80) != 0)
        fn_02388CD8(1);
    return bit;
}
