#pragma optimize_for_size on
/* Unsigned x / base for the formatter's radixes: shifts for 2, 8 and 16;
   for 10 a 0x1999 reciprocal multiply on the two half-words, then a
   correction loop. Any other base gives 0. func_0206a928 uses it for its
   digit loop and the %e exponent. */
unsigned int func_0206c93c(unsigned int x, unsigned int base)
{
    unsigned int q, lo, hi;
    switch (base) {
    case 2:
        return x >> 1;
    case 8:
        return x >> 3;
    case 0x10:
        return x >> 4;
    case 0xa:
        lo = x & 0xffff;
        hi = x >> 16;
        q = hi * 0x1999 + ((hi * 0x9999 + lo * 0x1999) >> 16);
        if (x >= 0xa) {
            base = x - 9;
            x = 0xa;
            while (q * x < base) {
                q++;
            }
        }
        return q;
    }
    return 0;
}
