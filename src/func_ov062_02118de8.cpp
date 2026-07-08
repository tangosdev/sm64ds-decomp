//cpp
extern "C" int _ZN9Animation8FinishedEv(void *a);
extern "C" void func_ov062_02117994(char *c, int idx);
extern "C" int _ZNK9Animation13GetFrameCountEv(void *a);
extern "C" int _ZNK9Animation12WillHitFrameEi(void *a, int f);
extern "C" int func_ov062_02118058(char *c);
extern "C" int RandomIntInternal(int *seed);
extern "C" int RNG_STATE;

extern "C" void func_ov062_02118de8(char *c)
{
    *(int*)(c + 0x98) = 0;
    if (*(unsigned char*)(c + 0x398) == 2) {
        if (_ZN9Animation8FinishedEv(c + 0x350) == 0) return;
        func_ov062_02117994(c, 4);
        return;
    }
    if (_ZNK9Animation12WillHitFrameEi(c + 0x350, (unsigned short)(_ZNK9Animation13GetFrameCountEv(c + 0x350) - 1)) != 0) {
        unsigned short *hp = (unsigned short*)(((int)c + 0x3c4) & 0xFFFFFFFFFFFFFFFF);
        *hp += 1;
    } else {
        func_ov062_02118058(c);
        return;
    }
    func_ov062_02117994(c, 0);
    if (*(int*)(c + 0x390) == 1)
        *(int*)(c + 0x38c) = 5;
    else
        *(int*)(c + 0x38c) = 2;
    if (((unsigned int)RandomIntInternal(&RNG_STATE) >> 16) & 0x8000) {
        int r = ((unsigned int)RandomIntInternal(&RNG_STATE) >> 16) & 0x1fff;
        *(short*)(c + 0x3c2) = *(short*)(c + 0x94) - r;
    } else {
        int r = ((unsigned int)RandomIntInternal(&RNG_STATE) >> 16) & 0x1fff;
        *(short*)(c + 0x3c2) = *(short*)(c + 0x94) + r;
    }
    func_ov062_02118058(c);
}
