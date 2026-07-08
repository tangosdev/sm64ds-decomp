extern int RandomIntInternal(void* seed);
extern int RNG_STATE[];
int func_ov006_021237c8(int x) {
    int r;
    if (x < 5) {
        r = (int)(((unsigned int)RandomIntInternal(RNG_STATE) & 0x7fffffff) >> 0x13);
        return r < 0xc00 ? 4 : 5;
    }
    r = (int)(((unsigned int)RandomIntInternal(RNG_STATE) & 0x7fffffff) >> 0x13);
    return r < 0xc00 ? 6 : 7;
}
