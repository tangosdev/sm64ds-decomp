extern int RandomIntInternal(int *seed);
extern int RNG_STATE;

int func_ov006_0212373c(int a)
{
    int r;
    if (a < 5) {
        r = (int)(((unsigned int)RandomIntInternal(&RNG_STATE) & 0x7fffffff) >> 0x13);
        if (r < 0xc00)
            return 4;
        if (r < 0xe00)
            return 5;
        return 8;
    }
    r = (int)(((unsigned int)RandomIntInternal(&RNG_STATE) & 0x7fffffff) >> 0x13);
    if (r < 0xc00)
        return 6;
    if (r < 0xe00)
        return 8;
    return 7;
}
