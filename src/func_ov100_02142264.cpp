//cpp
#include "common.h"

extern "C" {
extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void* thiz);
extern void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void* thiz, void* sm, void* mtx, int f, int g, unsigned int h);
extern int IDENTITY_MATRIX4X3[];


void func_ov100_02142264(char* c)
{
    *(Matrix4x3*)(c + 0x2e8) = *(Matrix4x3*)IDENTITY_MATRIX4X3;
    *(int*)(c + 0x30c) = *(int*)(c + 0x5c) >> 3;
    *(int*)(c + 0x310) = *(int*)(c + 0x60) >> 3;
    *(int*)(c + 0x314) = *(int*)(c + 0x64) >> 3;
    *(Matrix4x3*)(c + 0x344) = *(Matrix4x3*)(c + 0x2e8);
    int k = 0x32;
    *(int*)(c + 0x36c) = (*(int*)(c + 0x3ac) * k + *(int*)(c + 0x60)) >> 3;
    if (_ZNK10dBgCh_Actr10IsOnGroundEv(c + 0x110) != 0)
        k += 0x28;
    else
        k += 0x190;
    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        c, c + 0x31c, c + 0x344, *(int*)(c + 0x3ac) * 0xc8, k * *(int*)(c + 0x3ac), 0xf);
}
}
