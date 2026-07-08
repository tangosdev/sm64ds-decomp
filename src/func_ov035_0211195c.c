//cpp
// NONMATCHING: different op / idiom (div=30). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern "C" int DecIfAbove0_Short(char *p);
extern "C" int RandomIntInternal(char *p);
extern "C" void func_020393a4(int *p, int v);
extern "C" void func_02039394(int *p, int v);
extern "C" void func_ov035_021118a8(char *t);
extern "C" int func_ov035_02111798(char *c);
extern "C" int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(char *c, int a, int b);
extern "C" void _ZN8Platform19UpdateClsnPosAndRotEv(char *c);
extern unsigned char LEVEL_SPECIFIC_SETTING[];
extern short data_ov035_02112b80[];
extern int RNG_STATE[];
extern "C" int func_ov035_0211195c(char *c) {
    unsigned char idx = LEVEL_SPECIFIC_SETTING[0];
    *(short*)(c+0x92) = data_ov035_02112b80[idx];
    if (idx == 2) {
        if (DecIfAbove0_Short(c+0x320) == 0) {
            int r = (unsigned short)((unsigned)RandomIntInternal((char*)RNG_STATE) >> 16);
            if ((unsigned)r >= 0x7fff) *(signed char*)(c+0x31e) = 1;
            else *(signed char*)(c+0x31e) = -1;
            *(short*)(c+0x320) = (short)((r % 4 + 1) * 0x1e);
            *(unsigned short*)(c+0x322) = *(unsigned short*)(c+0x320);
        } else {
            if ((int)*(unsigned short*)(c+0x320) >= (int)*(unsigned short*)(c+0x322) - 5) {
                *(short*)(c+0x92) = 0;
            } else {
                short *q = (short*)(c+0x92);
                *q = (short)(*q * *(signed char*)(c+0x31e));
            }
        }
    }
    func_020393a4((int*)(c+0x124), 0x180000);
    func_02039394((int*)(c+0x124), 0x1000);
    {
        short *s = (short*)(c+0x8c);
        *s = (short)(*s + *(short*)(c+0x92));
    }
    func_ov035_021118a8(c);
    func_ov035_02111798(c);
    if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(c, 0x180000, 0x1000))
        _ZN8Platform19UpdateClsnPosAndRotEv(c);
    return 1;
}
