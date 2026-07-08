//cpp
// NONMATCHING: different op / idiom (div=43). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
struct C;
typedef void (C::*PMF)();
struct Entry { PMF pmf; };
extern Entry data_ov064_0211c750[];
extern short SINE_TABLE[];
extern "C" {
extern int _Z14ApproachLinearRiii(int* a, int b, int c);
extern void _ZN8Platform21UpdateModelPosAndRotYEv(void*);
extern int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(void*, int, int);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void*);

int func_ov064_02117d24(char* c) {
    int idx = *(unsigned char*)(c + 0x33b);
    int saved;
    (((C*)c)->*data_ov064_0211c750[idx].pmf)();
    *(unsigned short*)(c + 0x338) += 1;
    if (idx != *(unsigned char*)(c + 0x33b)) {
        *(short*)((c + 0x300) + 0x38) = 0;
    }
    if (_Z14ApproachLinearRiii((int*)(c + 0x320), *(unsigned char*)(c + 0x33a) ? -0x28000 : 0, 0x5000)) {
        *(short*)(c + 0x328) += 0xa00;
        *(int*)(c + 0x324) = SINE_TABLE[(*(unsigned short*)((c + 0x300) + 0x28) >> 4) * 2] * (short)0xa;
    }
    saved = *(int*)(c + 0x60);
    *(int*)(c + 0x60) = *(int*)(c + 0x324) + (*(int*)(c + 0x330) + *(int*)(c + 0x320));
    _ZN8Platform21UpdateModelPosAndRotYEv(c);
    if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(c, 0, 0)) {
        _ZN8Platform19UpdateClsnPosAndRotEv(c);
    }
    *(int*)(c + 0x60) = saved;
    *(unsigned char*)(c + 0x33a) = 0;
    return 1;
}
}
