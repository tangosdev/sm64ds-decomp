//cpp
struct C;
typedef void (C::*PMF)();
struct Entry { PMF pmf; };
extern Entry data_ov064_0211c750[];
extern short data_02082214[];
extern "C" {
extern int _Z14ApproachLinearRiii(int* a, int b, int c);
extern void _ZN8Platform21UpdateModelPosAndRotYEv(void*);
extern int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(void*, int, int);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void*);
}

extern "C" int func_ov064_02117d24(char* c) {
    int idx = *(unsigned char*)(c + 0x33b);
    (((C*)c)->*data_ov064_0211c750[idx].pmf)();
    unsigned short* p338 = (unsigned short*)(((long long)(int)(c + 0x338)) & 0xFFFFFFFFFFFFFFFFLL);
    *p338 = (unsigned short)(*p338 + 1);
    if (idx != *(unsigned char*)(c + 0x33b)) {
        *(short*)((char*)c + 0x300 + 0x38) = 0;
    }
    int target = *(unsigned char*)(c + 0x33a) ? -0x28000 : 0;
    if (_Z14ApproachLinearRiii((int*)(c + 0x320), target, 0x5000)) {
        short* pAng = (short*)(((long long)(int)(c + 0x328)) & 0xFFFFFFFFFFFFFFFFLL);
        *pAng = (short)(*pAng + 0xa00);
        unsigned short h = *(unsigned short*)((char*)c + 0x300 + 0x28);
        short s = data_02082214[(h >> 4) * 2];
        short ten = 10;
        *(int*)(c + 0x324) = s * ten;
    }
    int t330 = *(int*)(c + 0x330);
    int t320 = *(int*)(c + 0x320);
    int t324 = *(int*)(c + 0x324);
    int saved = *(int*)(c + 0x60);
    *(int*)(c + 0x60) = t324 + (t330 + t320);
    _ZN8Platform21UpdateModelPosAndRotYEv(c);
    if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(c, 0, 0))
        _ZN8Platform19UpdateClsnPosAndRotEv(c);
    *(int*)(c + 0x60) = saved;
    *(unsigned char*)(c + 0x33a) = 0;
    return 1;
}
