//cpp
// @symbol func_ov100_0214117c
/* recovered: shared common types, declarations from a shared header */
#include "decl_Actor.h"
/* recovered: shared common types */
#include "common.h"
typedef short s16;


extern "C" {
extern void _ZN9ActorBase18MarkForDestructionEv(void* c);
extern int _ZN5Actor13DistToCPlayerEv(void* c);
extern void _Z14ApproachLinearRiii(int* val, int target, int step);
extern s16 Vec3_HorzAngle(const Vector3* a, const Vector3* b);
extern int RandomIntInternal(int* seed);
extern void _Z14ApproachLinearRsss(s16* val, s16 target, s16 step);
extern int data_0209e650;
}

extern "C" void func_ov100_0214117c(char* c)
{
    s16 hAngle;

    if (!_ZN5Actor15IsPlayerInRangeEi(c, 0x5dc)) {
        _ZN9ActorBase18MarkForDestructionEv(c);
        return;
    }

    if (*(int*)(c + 0x3e8) > 0x6e && _ZN5Actor13DistToCPlayerEv(c) < 0xc8000 &&
        (unsigned char)(*(unsigned char*)(c + 0x3f0) + 0xff) <= 1) {
        *(int*)(c + 0x98) = 0;
        *(int*)(c + 0x3e8) = 0;
        *(int*)(c + 0x3e4) = 6;
        *(int*)(((long long)(int)(c + 0xb0))) &= ~0x10000;
    } else {
        _Z14ApproachLinearRiii((int*)(c + 0x98), 0x8000, 0x800);
    }

    if (*(int*)(c + 0x3e8) >= 0x3c) {
        hAngle = *(s16*)(c + 0x3ec);
    } else {
        hAngle = Vec3_HorzAngle((Vector3*)(c + 0x5c), (Vector3*)(c + 0x3d4));
    }

    {
        s16 target;
        if (*(int*)(c + 0x60) < *(int*)(c + 0x3d8) +
                (int)(((unsigned)RandomIntInternal(&data_0209e650) >> 16 & 0xfff) * 0x32 + 0x32000)) {
            target = -0x2000;
        } else {
            target = 0x2000;
        }
        _Z14ApproachLinearRsss((s16*)(c + 0x92), target, 0x190);
    }

    {
        _Z14ApproachLinearRsss((s16*)(c + 0x94), hAngle,
            (s16)((((unsigned)RandomIntInternal(&data_0209e650) >> 16) % 800) + 0x190));
    }
}
