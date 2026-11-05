#pragma opt_common_subs off
/* The 11-bit biased exponent of a double, read a byte at a time through
   u16 loads (bytes 7 and 6). 0x3ff is 1.0. */
int func_0206c8b4(double x)
{
    int hi, lo;
    int p;

    p = (int)&x + 7;
    if (p & 1)
        hi = (*(unsigned short *)(p - 1) & 0xff00) >> 8;
    else
        hi = *(unsigned short *)p & 0xff;

    p = (int)&x + 6;
    if (p & 1)
        lo = (*(unsigned short *)(p - 1) & 0xff00) >> 8;
    else
        lo = *(unsigned short *)p & 0xff;

    return ((hi & 0x7f) << 4) + (lo >> 4);
}
