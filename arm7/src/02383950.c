extern int fn_02383A78(void);

unsigned fn_02383950(int scale, int steps) {
    int quot;
    int i;
    unsigned int sample;
    unsigned long long wide;
    unsigned long long prod;
    int sh;
    unsigned int lo;
    unsigned int hi;
    quot = 0;
    i = -steps;
    while (i < 0) {
        --quot;
        i += 768;
    }
    while (i >= 768) {
        ++quot;
        i -= 768;
    }
    sample = (unsigned int)fn_02383A78();
    hi = 0;
    lo = 0x10000;
    wide = (unsigned long long)sample + lo;
    wide += (unsigned long long)hi << 32;
    prod = wide * (unsigned long long)(long long)scale;
    sh = quot - 16;
    if (sh <= 0) {
        prod >>= (unsigned)(-sh);
    } else if (sh >= 32) {
        goto saturated;
    } else {
        if ((prod & (~0ULL << (32 - sh))) != 0)
            return 0xFFFF;
        prod <<= sh;
    }
    goto clamp;
saturated:
    return 0xFFFF;
clamp:
    if (prod < 0x10)
        prod = 0x10;
    else if (prod > 0xFFFF)
        prod = 0xFFFF;
    return (unsigned short)prod;
}
