#ifndef DAOBJBK_LIFT_C_H
#define DAOBJBK_LIFT_C_H

#include "types.h"
#include "dBgW_KcMbg.h"

/* Derives from dBgActor_c: the destructor stores this class's vtable, then
 * dBgActor_c's -- inlined -- then destroys the dBgW_KcMbg at 0x124 and
 * the dExtSimpleModel_c at 0xd4 before chaining to dActor_c. All three belong to dBgActor_c.
 * Everything this header used to restate below 0x31e was dActor_c's and
 * dBgActor_c's, and is inherited now.
 *
 * SIZE IS THE OBSERVED FIELD SPAN, rounded up. It guards this declaration; it
 * is not independent evidence about the ROM.
 */

#include "dBgActor_c.h"
#include "dExtShadowModel_c.h"

struct daObjBk_Lift_c : dBgActor_c {
    u8 pad_31e[0x2];
    dExtShadowModel_c mShadowModel; /* 0x320 */
    /* Shadow transform. UpdateShadow copies mModel.mat4x3 here, then
       overwrites the translation row (0x36c..0x374) before DropShadowScaleXYZ. */
    Matrix4x3 mShadowMat; /* 0x348 */
    s32 mFloorPosY;       /* 0x378 */
    s32 mMinPosY;         /* 0x37c */
    s32 mMaxPosY;         /* 0x380 */
    s32 mShadowOffsetX;   /* 0x384 */
    s32 mShadowOffsetY;   /* 0x388 */
    s32 mSoundID;         /* 0x38c */
    u8 mMoveTimer;        /* 0x390 */
    u8 mJustSteppedOn;    /* 0x391 */
    u8 mMove;             /* 0x392 */

    /* --- vtable --- */
    /* Inline empty dtor: mwccarm emits D1 then D0 and no D2, which is the
       order the cartridge has -- D1 at 0x02112290, D0 at 0x021122dc. Same
       form as include/daObjBk_Rotebar_c.h in this overlay. */
    virtual ~daObjBk_Lift_c() {}

    int InitResources();
    int CleanupResources();
    int Behavior();
    int Render();
    /* Not vtable slots. */
    void UpdateShadow();
    /* Mesh-collider stood-on callback pair: arms a kind-2 ride. */
    static int StoodOnCallback(dBgW *, dActor_c *, dActor_c *);
    int OnStoodOn(dActor_c *);
    virtual void OnHitByMegaChar(daPly_c &player); /* slot 27 */
    /* dBgActor_c's own slot, overridden here: _ZTV14daObjBk_Lift_c+0x7c relocates to
       0x0211233c while _ZTV10dBgActor_c+0x7c relocates to _ZN10dBgActor_c4KillEv. An
       override, so it adds no slot and no field. */
    virtual void Kill(); /* slot 31 */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjBk_Lift_c_size_must_be_0x394[sizeof(daObjBk_Lift_c) == 0x394 ? 1 : -1];
#endif

#endif /* DAOBJBK_LIFT_C_H */
