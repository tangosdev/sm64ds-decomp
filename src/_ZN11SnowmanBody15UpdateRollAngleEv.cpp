//cpp
#include "SnowmanBody.h"

typedef int Fix12i;
extern "C" Fix12i _ZN4cstd4fdivEii(Fix12i a, Fix12i b);

void SnowmanBody::UpdateRollAngle()
{
    char *c = (char *)this;
    int d = (int)(((long long)(*(int *)(c + 0x398) << 1) *
                       0x3243F6A89LL + 0x80000000LL) >> 32);
    Fix12i q = _ZN4cstd4fdivEii(*(int *)(c + 0x98), d);
    *(short *)(c + 0x8c) = (short)(*(short *)(c + 0x8c) +
        (int)(((long long)q * 0xffff + 0x800) >> 12));
}
