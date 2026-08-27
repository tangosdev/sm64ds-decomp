//cpp
#include "types.h"
// @symbol _ZN11RollingRock13InitResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "RollingRock.h"
struct Vec3 { s32 x, y, z; };
struct dActor_c;

extern "C" {
    void _ZN5Model8LoadFileER13SharedFilePtr(void* shared);
    int _ZN9ModelBase7SetFileEP8BMD_Fileii(void* mb, void* bmd, int a, int b);
    int _ZN11ShadowModel12InitCylinderEv(void* sm);
    void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void* w, void* a, int r, int rr, void* v, void* vv);
    void _ZN10dBgCh_Actr13SetLimMovFlagEv(void* w);
    void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(void* c, void* a, void* pos, int r, int rr, u32 e, u32 f);
    // r0=this, r1=actor, r2=&pos, r3=r(Fix12i), [sp]=rr, [sp+4]=e, [sp+8]=f
    u8 _ZN8dActor_c9TrackStarEjj(void* a, u32 x, u32 y);
}

struct SharedFilePtr { void* file; void* bmd; };
extern SharedFilePtr data_ov021_02114a50;
extern short data_02082214[];

int RollingRock::InitResources()
{
    u8* c = (u8*)((void*)this);
    *(u8*)(c+0x3be) = (u8)(*(u32*)(c+8) & 0xf);
    *(u8*)(c+0x3c1) = 0xff;
    *(signed char*)(c+0x3c0) = -1;
    *(u8*)(c+0x3bf) = 0;

    _ZN5Model8LoadFileER13SharedFilePtr(&data_ov021_02114a50);

    if (*(u8*)(c+0x3be) >= 2) {
        if (_ZN9ModelBase7SetFileEP8BMD_Fileii((void*)(c+0x110), data_ov021_02114a50.bmd, 1, 1) == 0)
            return 0;
        if (_ZN11ShadowModel12InitCylinderEv((void*)(c+0x160)) == 0)
            return 0;

        _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_((void*)(c+0x1f8), ((void*)this), 0x12c000, 0, 0, 0);
        _ZN10dBgCh_Actr13SetLimMovFlagEv((void*)(c+0x1f8));
        *(s32*)(c+0x3b8) = 0;

        if (*(u8*)(c+0x3be) == 4) {
            Vec3 pos;
            *(u8*)(c+0x3c1) = (u8)((*(u32*)(c+8) >> 8) & 0xf);
            pos.x = 0; pos.y = 0; pos.z = 0;
            _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj((void*)(c+0x1b8), ((void*)this), &pos, 0x10e000, 0x226000, 0x200004, 0x3c0);
        } else {
            Vec3 pos;
            pos.x = 0; pos.y = 0; pos.z = 0;
            _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj((void*)(c+0x1b8), ((void*)this), &pos, 0xf3000, 0x226000, 0x200004, 0x3c0);

            if (*(u8*)(c+0x3be) == 3) {
                *(u8*)(c+0x3c1) = (u8)((*(u32*)(c+8) >> 8) & 0xf);
            }
            *(s32*)(c+0x9c) = -0x4000;
            *(s32*)(c+0xa0) = -0x28000;
            *(s32*)(c+0x98) = 0x1e000;
            *(s32*)(c+0xa4) = data_02082214[(*(u16*)(c+0x94)>>4)<<1] * 0x1e;
            *(s32*)(c+0xac) = data_02082214[((*(u16*)(c+0x94)>>4)<<1)+1] * 0x1e;
        }
    } else {
        if (*(u8*)(c+0x3be) == 1) {
            *(u8*)(c+0x3c1) = (u8)((*(u32*)(c+8) >> 8) & 0xf);
        }
    }

    if (*(u8*)(c+0x3be) != 1 && *(u8*)(c+0x3c1) != 0xff) {
        *(u8*)(c+0x3c0) = _ZN8dActor_c9TrackStarEjj(((void*)this), *(u8*)(c+0x3c1), 2);
    }
    *(s32*)(c+0x3c4) = 0;
    *(s32*)(c+0x10c) = 0;
    *(s32*)(c+0x3b4) = 0;
    *(u8*)(c+0x3c2) = 0;
    return 1;
}
