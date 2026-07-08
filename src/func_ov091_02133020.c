extern int RandomIntInternal(int *seed);
extern int RNG_STATE;
void func_ov091_02133020(char *c)
{
    (*(int *)(((int)c + 0x60) & 0xFFFFFFFFFFFFFFFF)) += 0xa000;
    if (*(int *)(c + 0x60) < *(int *)(c + 0x390))
        return;
    *(int *)(c + 0x60) = *(int *)(c + 0x390);
    *(int *)(c + 0x398) = 1;
    *(unsigned char *)(c + 0x39e) =
        (unsigned char)(((unsigned int)RandomIntInternal(&RNG_STATE) >> 0x10) % 0x1e + 0xa);
}
