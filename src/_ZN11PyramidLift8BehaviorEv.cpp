//cpp
// NONMATCHING: register allocation (div=52). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern "C" {
extern short data_02082214[];
extern void _ZN8Platform21UpdateModelPosAndRotYEv(void*);
extern int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(void*, int, int);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void*);
}

extern "C" int _ZN11PyramidLift8BehaviorEv(char* c)
{
    switch (*(unsigned char*)(c + 0x3f6)) {
    case 0:
        if (*(unsigned char*)(c + 0x3f7) != 0) {
            *(unsigned char*)(c + 0x3f6) = 1;
            *(short*)(c + 0x3f4) = 0;
        }
        break;
    case 1: {
        unsigned short* pa = (unsigned short*)(c + 0x3f4);
        int idx = (int)(*pa << 0x1c) >> 0x10;
        idx = (int)((unsigned)(idx << 0x10) >> 0x10) >> 4;
        int s = data_02082214[idx << 1];
        int d = (int)(((long long)s * 0xa + 0x800) >> 0xc);
        *(int*)(c + 0x60) = *(int*)(c + 0x374) + d;
        if (*pa == 8) {
            *(unsigned char*)(c + 0x3f6) = 2;
            *(int*)(c + 0xa8) = -0xa000;
        }
        *pa = *pa + 1;
        break;
    }
    case 2: {
        int v = *(int*)(c + 0x60);
        int idx = *(unsigned char*)(c + 0x3f8);
        int* p = (int*)(c + idx * 0xc + 0x380);
        int lim = *p + 0x14000;
        if (v <= lim) {
            *(unsigned char*)(c + 0x3f8) = *(unsigned char*)(c + 0x3f8) + 1;
        }
        *(int*)(c + 0x60) = *(int*)(c + 0x60) + *(int*)(c + 0xa8);
        if (*(int*)(c + 0x60) < 0x80000) {
            *(int*)(c + 0x60) = 0x80000;
            *(unsigned char*)(c + 0x3f6) = 3;
            *(short*)(c + 0x3f4) = 0;
        }
        break;
    }
    case 3: {
        int idx = (int)(*(unsigned short*)(c + 0x3f4) << 0x1c) >> 0x10;
        idx = (int)((unsigned)(idx << 0x10) >> 0x10) >> 4;
        int s = data_02082214[idx << 1];
        int d = (int)(((long long)s * 0xa + 0x800) >> 0xc);
        *(int*)(c + 0x60) = d + 0x80000;
        if (*(unsigned short*)(c + 0x3f4) >= 8) {
            *(int*)(c + 0xa8) = 0;
            *(int*)(c + 0x60) = 0x80000;
        }
        *(unsigned short*)(c + 0x3f4) = *(unsigned short*)(c + 0x3f4) + 1;
        _ZN8Platform21UpdateModelPosAndRotYEv(c);
        if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(c, 0, 0))
            _ZN8Platform19UpdateClsnPosAndRotEv(c);
        *(unsigned char*)(c + 0x3f7) = 0;
        break;
    }
    }
    return 1;
}
