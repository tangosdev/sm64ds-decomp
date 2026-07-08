extern int RandomIntInternal(int *seed);
extern int RNG_STATE;

int func_ov022_02112710(char *c)
{
    unsigned int r = (unsigned int)RandomIntInternal(&RNG_STATE) >> 8;
    int *p98;
    int *pa8;
    *(short *)(c + 0x110) = 0x3c;
    *(short *)(c + 0x94) = (short)((r & 0xf) << 0xc);
    *(int *)(c + 0x98) = (r & 0x1f) + 0x14;
    p98 = (int *)(((int)c + 0x98) & 0xFFFFFFFFFFFFFFFFULL);
    *p98 = *p98 << 0xc;
    *(int *)(c + 0x9c) = -0x4000;
    *(int *)(c + 0xa8) = (r & 0x3f) + 0x28;
    pa8 = (int *)(((int)c + 0xa8) & 0xFFFFFFFFFFFFFFFFULL);
    *pa8 = *pa8 << 0xc;
    return 1;
}
