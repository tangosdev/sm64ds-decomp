#ifndef DAOBJCTMECHA09_C_H
#define DAOBJCTMECHA09_C_H

#include "types.h"
#include "math/Matrix.h"

/* Derives from dBgActor_c: the destructor stores this class's vtable, then
 * dBgActor_c's -- inlined -- then destroys the dBgW_KcMbg at 0x124 and
 * the dExtSimpleModel_c at 0xd4 before chaining to dActor_c. All three belong to dBgActor_c.
 * Everything this header used to restate below 0x31e was dActor_c's and
 * dBgActor_c's, and is inherited now.
 *
 * SIZE IS THE OBSERVED FIELD SPAN, rounded up. It guards this declaration; it
 * is not independent evidence about the ROM.
 */

#ifdef __cplusplus

#include "dBgActor_c.h"
#include "dExtShadowModel_c.h"

struct daObjCtMecha09_c : dBgActor_c {
    u8 pad_31e[0x2];
    s32 mStartPosY; /* 0x320 */
    s32 mEndPosY;   /* 0x324 */
    s8 mDirection; /* 0x328 -- +1/-1; Behavior reads it ldrsb and negates it at the travel limits */
    u8 pad_329[0x1];
    u16 mLegTimer;  /* 0x32a -- DecIfAbove0_Short countdown for the current leg (setting 2) */
    u16 mLegLength; /* 0x32c -- the leg's full length; the last five frames hold still */
    u8 pad_32e[0x2];
    s32 mGroundY; /* 0x330 -- InitResources' dBgCh_Gnd raycast result, falling back to the probe height */
    dExtShadowModel_c mShadowModel; /* 0x334 */

    /* --- vtable --- */
    virtual ~daObjCtMecha09_c();

    /* Behavior overrides fBase_c::Behavior: _ZTV16daObjCtMecha09_c slot 6 holds
       _ZN16daObjCtMecha09_c8BehaviorEv (ov065 0x0211bd8c). No `virtual` keyword,
       matching the overrides beside it: a derived declaration of a base virtual
       overrides whether or not it repeats the word. */
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();

    /* Drops the cuboid shadow onto mGroundY; ov065 0x0211bc88. */
    void UpdateShadow();

    /* Tail padding. The field span stops short of the real size:
       daObjCtMecha09_c_classInit (historical alias TTC_MovingBeam_Spawn)
       calls fBase_c::operator new(0x38c), read off the retail
       instruction. A span is only a LOWER BOUND. */
    Matrix4x3 mShadowMat; /* 0x35c */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjCtMecha09_c_size_must_be_0x38c[sizeof(daObjCtMecha09_c) == 0x38c ? 1 : -1];
#endif

#endif /* __cplusplus */

#endif /* DAOBJCTMECHA09_C_H */
