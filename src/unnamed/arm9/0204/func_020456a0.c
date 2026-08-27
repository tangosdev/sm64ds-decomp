extern int _ZN4cstd3absEi(int x);

struct B3 { unsigned char b[3]; };
extern const struct B3 data_02099f84;

unsigned short func_020456a0(int a0, int a1, int shift, int a3, short *tbl)
{
    int i, frac;
    short lo, hi, srange, sfrac;
    struct B3 buf;

    if (a3 == 0)
        return ((unsigned short *)tbl)[0];
    if (shift == 0)
        return ((unsigned short *)tbl)[a1];

    i = a1 >> shift;
    {
        int rounded = ((a0 - 1) >> shift) << shift;
        if (a1 >= rounded)
            return ((unsigned short *)tbl)[i + (a1 - rounded)];
    }

    frac = a1 - (i << shift);
    lo = tbl[i];
    hi = tbl[i + 1];

    buf = data_02099f84;

    sfrac = (short)frac;
    srange = (short)(buf.b[shift] - sfrac);

    if (sfrac == 0)
        return (unsigned short)lo;

    if (_ZN4cstd3absEi(lo - hi) >= 0x800) {
        if (lo > hi)
            hi = (short)(hi + 0x1000);
        else
            lo = (short)(lo + 0x1000);
        return (unsigned short)((unsigned short)((hi * sfrac + lo * srange) >> shift) & 0xfff);
    }
    return (unsigned short)((lo * srange + hi * sfrac) >> shift);
}
