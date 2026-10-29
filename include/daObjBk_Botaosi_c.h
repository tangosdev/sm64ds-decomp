#ifndef DAOBJBK_BOTAOSI_C_H
#define DAOBJBK_BOTAOSI_C_H

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

struct daObjBk_Botaosi_c : dBgActor_c {
    u8 pad_31e[0x2];
    dExtShadowModel_c mShadowModel; /* 0x320 */
    /* The shadow's transform: UpdateShadow builds it with
       Matrix4x3_FromRotationY and writes its translation row (0x36c..0x374)
       before handing it to dActor_c::DropShadowScaleXYZ with mShadowModel. */
    Matrix4x3 mShadowMat; /* 0x348 */
    /* The point 0x32000 units in front of the plank: InitResources rotates
       (0, 0, 0x32000) by mAngleY through data_020a0e68 and adds the plank's own
       position. mFrontFloorY below is the ground height under it. */
    s32 mFrontPosX;    /* 0x378 */
    s32 mFrontPosY;    /* 0x37c */
    s32 mFrontPosZ;    /* 0x380 */
    s32 mFrontFloorY;  /* 0x384 */
    s32 mOriginalPosY; /* 0x388 */
    s32 mJumpSpeed;    /* 0x38c */
    s16 mWobbleAng;    /* 0x390 */
    s16 mFallAngVel;   /* 0x392 */
    s16 mWobbleTimer;  /* 0x394 */
    s8 mKnockDir;      /* 0x396 */
    u8 mState;         /* 0x397 */
    /* daObjBk_Botaosi_c_classInit, the one factory storing _ZTV17daObjBk_Botaosi_c
       (ov015:0x02114420), calls fBase_c::operator new(0x39c). The field span
       stopping at 0x398 is a lower bound, not the size. */
    u8 pad_398[0x4]; /* 0x398, to the ROM's 0x39c */

    /* --- vtable --- */
    /* The key function: declared first and defined first in
       src/game/actors/d_a_obj_bk_botaosi.cpp, so that TU emits the vtable and RTTI.
       Under its `#pragma defer_codegen off` the out-of-line definition comes
       out D1 (0x02111314) then D0 (0x02111360), the cartridge's order. */
    virtual ~daObjBk_Botaosi_c();

    int InitResources();
    int CleanupResources();
    int Behavior();
    int Render();

    void HandleHit(dActor_c *other); /* OnKicked / OnAttacked2 */
    void UpdateShadow();
    void UpdateModelMatrix();

    virtual int OnAttacked2(dActor_c &other);     /* slot 23 */
    virtual void OnKicked(dActor_c &other);       /* slot 24 */
    virtual void OnHitByMegaChar(Player &player); /* slot 27 */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjBk_Botaosi_c_size_must_be_0x39c[sizeof(daObjBk_Botaosi_c) == 0x39c ? 1 : -1];
#endif

#endif /* DAOBJBK_BOTAOSI_C_H */
