#include "types.h"
extern int _ZN4cstd5atan2E5Fix12IiES1_(Fix12i y, Fix12i x);
extern int AngleDiff(int a, int b);
extern int func_ov002_020c0cbc(void* c);

int func_ov002_020dd2f4(char* c)
{
    int atan;

    if (!(*(unsigned char*)(c + 0x6e9) & 2)) return 0;

    atan = _ZN4cstd5atan2E5Fix12IiES1_(*(int*)(c + 0x560), *(int*)(c + 0x568));
    if (AngleDiff(atan, *(s16*)(c + 0x94)) < 0x6000) return 0;

    if (func_ov002_020c0cbc(c) != 0) {
        *(int*)(c + 0x98) = (int)(((long long)*(int*)(c + 0x98) * 0xe66 + 0x800) >> 12);
        if (*(int*)(c + 0x98) < 0x4000) *(int*)(c + 0x98) = 0x4000;
        *(s16*)(c + 0x94) = atan - (s16)(*(s16*)(c + 0x94) - atan) + 0x8000;
    }
    return 0;
}
