//cpp
extern "C" {
extern void _ZN8Platform21UpdateModelPosAndRotYEv(void*);
extern int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(void*, int, int);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void*);
extern int _Z14ApproachLinearRsss(short* dst, short to, short step);
extern unsigned short DecIfAbove0_Short(unsigned short* p);
extern int RandomIntInternal(int* seed);
extern unsigned char LEVEL_SPECIFIC_SETTING[];
extern short data_ov035_02111ef4[][4];
extern short data_ov035_02111ef0[];
extern int RNG_STATE[];
}

extern "C" int func_ov035_021112ec(char* c)
{
    if (LEVEL_SPECIFIC_SETTING[0] == 3) {
        _ZN8Platform21UpdateModelPosAndRotYEv(c);
        if (*(int*)(c + 0x32c) == 0 && _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(c, 0, 0))
            _ZN8Platform19UpdateClsnPosAndRotEv(c);
        return 1;
    }

    if (_Z14ApproachLinearRsss((short*)(c + 0x8e), *(short*)(c + 0x322), 0xc8) != 0 &&
        DecIfAbove0_Short((unsigned short*)(c + 0x31e)) == 0) {
        short* p = (short*)(((int)c + 0x322) & 0xFFFFFFFFFFFFFFFF);
        *p = *p + *(short*)(c + 0x324);
        unsigned char k = LEVEL_SPECIFIC_SETTING[0];
        *(short*)(c + 0x31e) = data_ov035_02111ef4[*(int*)(c + 0x32c)][k];
        if (k == 2) {
            int rnd = RandomIntInternal(RNG_STATE);
            if (DecIfAbove0_Short((unsigned short*)(c + 0x320)) == 0) {
                if ((unsigned int)rnd % 3 != 0) {
                    int r2 = (rnd & 3) * 0x3c;
                    *(short*)(c + 0x324) = data_ov035_02111ef0[*(int*)(c + 0x32c)];
                    *(short*)(c + 0x320) = r2 + 0x5a;
                } else {
                    *(short*)(c + 0x324) = -data_ov035_02111ef0[*(int*)(c + 0x32c)];
                    *(short*)(c + 0x320) = ((unsigned int)rnd % 3 + 1) * 0x1e;
                }
            }
            *(short*)(c + 0x31e) = (unsigned int)rnd % 3 * 0x14 + 0xa;
        }
    }

    _ZN8Platform21UpdateModelPosAndRotYEv(c);
    if (*(int*)(c + 0x32c) == 0 && _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(c, 0, 0))
        _ZN8Platform19UpdateClsnPosAndRotEv(c);
    return 1;
}
