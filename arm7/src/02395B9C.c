int fn_02395B9C(int a1)
{
    volatile unsigned short *p = (volatile unsigned short *)0x4808180;
    int r;
    while (*p & 1) {
    }
    *(volatile unsigned short *)0x480817E = a1;
    *(volatile unsigned short *)0x480817C = (unsigned int)a1 >> 16;
    do {
        r = *p & 1;
    } while (r != 0);
    return r;
}
