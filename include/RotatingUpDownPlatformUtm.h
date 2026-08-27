/* class RotatingUpDownPlatformUtm : dBgActor_c. Real C++ form.
 *
 * Base and size from the factory (src/game/actors/RotatingUpDownPlatformUtm/RotatingUpDownPlatformUtm_Spawn.c):
 * fBase_c::operator new(936) -- 0x3a8 -- then dBgActor_c::dBgActor_c(), then
 * stores _ZTV25RotatingUpDownPlatformUtm. No intermediate base: one non-base
 * vtable store, matching the D1 destructor below.
 *
 * dBgActor_c ends at 0x320 (include/dBgActor_c.h). Members below that offset
 * used to be restated here under placeholder names by the AUTO-GENERATED flat
 * header; they are dActor_c's/dBgActor_c's own storage now (mSpawnParam ->
 * fBase_c::param1, mActorID -> fBase_c::actorID, unk_074 -> dActor_c::mCamSpacePosX,
 * unk_0b0 -> dActor_c::mFlags), confirmed against include/dActor_c.h and
 * include/fBase_c.h and updated at every call site.
 */
#ifndef ROTATINGUPDOWNPLATFORMUTM_H
#define ROTATINGUPDOWNPLATFORMUTM_H
#include "types.h"
#include "dBgW_KcMbg.h"

#ifdef __cplusplus

#include "dBgActor_c.h"
#include "ShadowModel.h"

struct RotatingUpDownPlatformUtm : dBgActor_c {
    /* Named by the class's own destructor calling ShadowModel's D1 at +0x320
       -- a relocation the ROM build checks. Was a u8 marker. */
    ShadowModel mShadowModel;    /* 0x320 */
    u8  pad_348[0x30];
    s32 mSoundHandle;            /* 0x378 */
    s32 mGroundY;            /* 0x37c */
    s16 mSpawnAngleX;            /* 0x380 */
    s16 mSpawnAngleY;            /* 0x382 */
    s16 mSpawnAngleZ;            /* 0x384 */
    u8  pad_386[0x2];
    s32 mBasePosX;            /* 0x388 */
    s32 mBasePosY;            /* 0x38c */
    s32 mBasePosZ;            /* 0x390 */
    u8  mWaypointIndex;            /* 0x394 */
    u8  mVariant;            /* 0x395 */
    u8  pad_396[0x2];
    s32 mPlatform0;            /* 0x398 */
    s32 mPlatform1;            /* 0x39c */
    u8  mIsDead;            /* 0x3a0 */
    u8  pad_3a1[0x1];
    u8  mPitchBase;            /* 0x3a2 */
    u8  pad_3a3[0x1];
    s16 mPitchStep;            /* 0x3a4 */
    u8  pad_3a6[0x2];

    /* --- vtable --- */
    virtual ~RotatingUpDownPlatformUtm();

    /* Slot 31, dBgActor_c's own new virtual (include/dBgActor_c.h). Attributed
       by the vtable: _ZTV25RotatingUpDownPlatformUtm carries 0x02131070 at
       slot 31, matching config/arm9/overlays/ov091/relocs.txt
       (0x02134cd8 -> 0x02131070). NOT the key function: the destructor above
       is declared out of line and defined identically in both D1Ev.cpp and
       D0Ev.cpp, so those two TUs keep emitting the vtable -- checked with
       objisolate, not assumed. */
    virtual void Kill();              /* slot 31 */

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();

    /* Slot 27, dActor_c's combat-callback override (include/dActor_c.h).
       Attributed by the vtable: 0x02134cc8 -> 0x021310fc
       (config/arm9/overlays/ov091/relocs.txt). */
    void OnHitByMegaChar(Player &player);   /* slot 27 */
};

typedef char RotatingUpDownPlatformUtm_size_must_be_0x3a8[sizeof(RotatingUpDownPlatformUtm) == 0x3a8 ? 1 : -1];

#else

#include "Model.h"

/* The same object spelled flat, for the compiler-generated destructor, which
   lives in a C translation unit and can never be migrated. */
struct RotatingUpDownPlatformUtm {
    u8  pad_000[0x8];
    s32 mSpawnParam;            /* 0x008 */
    u16 mActorID;            /* 0x00c */
    u8  pad_00e[0x4e];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0xc];
    s32 mCamSpacePosX;           /* 0x074 */
    s32 mCamSpacePosY;           /* 0x078 */
    s32 mCamSpacePosZ;           /* 0x07c */
    s32 mScaleX;                 /* 0x080 */
    s32 mScaleY;                 /* 0x084 */
    s32 mScaleZ;                 /* 0x088 */
    s16 mAngleX;            /* 0x08c */
    s16 mAngleY;            /* 0x08e */
    s16 mAngleZ;            /* 0x090 */
    u8  pad_092[0x1e];
    u32 mFlags;             /* 0x0b0 */
    u8  pad_0b4[0x18];
    s8  mAreaId;            /* 0x0cc */
    u8  pad_0cd[0x7];
    Model mModel;            /* 0x0d4 */
    dBgW_KcMbg mMeshCollider;            /* 0x124 */
    u8  pad_2ec[0x14];
    u8  unk_300;            /* 0x300 */
    u8  pad_301[0x1f];
    u8  mShadowModel;            /* 0x320 */
    u8  pad_321[0x57];
    s32 mSoundHandle;            /* 0x378 */
    s32 mGroundY;            /* 0x37c */
    s16 mSpawnAngleX;            /* 0x380 */
    s16 mSpawnAngleY;            /* 0x382 */
    s16 mSpawnAngleZ;            /* 0x384 */
    u8  pad_386[0x2];
    s32 mBasePosX;            /* 0x388 */
    s32 mBasePosY;            /* 0x38c */
    s32 mBasePosZ;            /* 0x390 */
    u8  mWaypointIndex;            /* 0x394 */
    u8  mVariant;            /* 0x395 */
    u8  pad_396[0x2];
    s32 mPlatform0;            /* 0x398 */
    s32 mPlatform1;            /* 0x39c */
    u8  mIsDead;            /* 0x3a0 */
    u8  pad_3a1[0x1];
    u8  mPitchBase;            /* 0x3a2 */
    u8  pad_3a3[0x1];
    s16 mPitchStep;            /* 0x3a4 */
};

#endif /* __cplusplus */

#endif
