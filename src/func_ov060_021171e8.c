extern int RandomIntInternal(void *);
extern int RNG_STATE[];
void func_ov060_021171e8(char *c)
{
    unsigned int r = RandomIntInternal(RNG_STATE);
    *(short *)(c + 0x94) = r >> 0x10;
    *(int *)(c + 0xa8) = 0xa000;
    *(int *)(c + 0x98) = 0;
    *(int *)(c + 0x360) = 0x6000;
    *(int *)(c + 0x368) = 0x10;
}
