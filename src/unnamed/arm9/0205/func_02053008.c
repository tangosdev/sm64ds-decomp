void func_02053008(int n)
{
    volatile unsigned short *r208 = (volatile unsigned short *)0x4000208;
    volatile unsigned short *r2b0 = (volatile unsigned short *)0x40002b0;
    volatile unsigned int *r2b8 = (volatile unsigned int *)0x40002b8;
    unsigned short old;
    if (n > 0) {
        old = *r208;
        *r208 = 0;
        *r2b0 = 1;
        r2b8[0] = 0;
        r2b8[1] = n;
        *r208;
        *r208 = old;
    } else {
        old = *r208;
        *r208 = 0;
        *r2b0 = 1;
        r2b8[0] = 0;
        r2b8[1] = 0;
        *r208;
        *r208 = old;
    }
}
