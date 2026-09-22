//cpp
#include "types.h"
// @symbol _ZN17SlidingPlatformWf13InitResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "SlidingPlatformWf.h"
extern "C" {
    void* _ZN5Model8LoadFileER13SharedFilePtr(void* shared);
    void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* mb, void* bmd, int a, int b);
    void _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void* self);
    void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void* self);
    void* _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(void* shared);
    void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void* mc, void* kcl, void* mtx, int f, s16 s, void* clps);
    void func_020393d4(void* p, void* v);
}

struct SFP { void* file; void* b1; void* b2; };
extern SFP data_ov091_02135024[];    /* model file, stride 0xc */
extern char data_ov091_02135028[];   /* collision file, stride 0xc */
extern char data_ov091_0213502c[];   /* CLPS block, stride 0xc */
extern u16 data_ov091_02134514[];    /* per-variant heading offset */
extern u16 data_ov091_02134504[];    /* per-variant travel time */
extern "C" {
extern void _ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_();
}

/* Wing-of-the-Fortress sliding slab: it walks along its own heading, waits, then
   turns 180 degrees and walks back. Which slab it is comes from the actor ID. */
int SlidingPlatformWf::InitResources()
{
    u8* c = (u8*)((void*)this);
    u16 t = *(u16*)(c+0xc);
    switch (t) {
        case 0x37: mVariant = 6; break;
        case 0x7c: mVariant = 3; break;
        case 0x93: mVariant = 4; break;
        case 0x9b: mVariant = 2; break;
        case 0x8a: mVariant = 0; break;
        case 0x9a: mVariant = 1; break;
        case 0x92: mVariant = 5; break;
    }

    void* bmd = _ZN5Model8LoadFileER13SharedFilePtr(data_ov091_02135024[mVariant].file);
    _ZN9ModelBase7SetFileEP8BMD_Fileii((void*)(c+0xd4), bmd, 1, -1);

    /* Heading to slide along: the spawn angle plus this slab's own offset,
       unless the spawn supplies one of its own. */
    *(u16*)(c+0x94) = (u16)(*(s16*)(c+0x8e) + data_ov091_02134514[mVariant]);

    if (*(s16*)(c+0x8c) != 0) {
        *(u16*)(c+0x94) = (u16)(*(s16*)(c+0x8e) + *(s16*)(c+0x8c));
    }

    mMoveTimer = data_ov091_02134504[mVariant];
    *(s32*)(c+0x98) = 0xa000;
    mBasePosX = *(s32*)(c+0x5c);
    mBasePosY = *(s32*)(c+0x60);
    mBasePosZ = *(s32*)(c+0x64);
    _ZN10dBgActor_c21UpdateModelPosAndRotYEv(((void*)this));
    _ZN10dBgActor_c19UpdateClsnPosAndRotEv(((void*)this));

    /* Slab 6 gets the full-size collider; the rest are scaled to 0x199. */
    if (mVariant == 6) {
        int oi = mVariant * 0xc;
        void* kcl = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(*(void**)(data_ov091_02135028+oi));
        _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            (void*)(c+0x124), kcl, (void*)(c+0x2ec), 0x1000, *(s16*)(c+0x8e), *(void**)(data_ov091_0213502c+oi));
    } else {
        int oi = mVariant * 0xc;
        void* kcl = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(*(void**)(data_ov091_02135028+oi));
        _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            (void*)(c+0x124), kcl, (void*)(c+0x2ec), 0x199, *(s16*)(c+0x8e), *(void**)(data_ov091_0213502c+oi));
    }
    func_020393d4((void*)(c+0x124), (void*)_ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);
    return 1;
}
