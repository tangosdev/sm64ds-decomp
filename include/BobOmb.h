#ifndef BOBOMB_H
#define BOBOMB_H

#include "types.h"

/* Derives from dEnemyBase_c, on the evidence of its own destructor: `_ZN6BobOmbD1Ev`
 * stores this vtable, destroys its members in reverse declaration order, then
 * calls `dEnemyBase_c::~dEnemyBase_c`. Everything this header used to restate below 0x110
 * belongs to that chain and is inherited now.
 *
 * The members close exactly on one another:
 *
 *     0x110 dCcAc_c         0x34   -> 0x144
 *     0x144 dBgCh_Actr               0x1bc  -> 0x300
 *     0x300 ModelAnim                  0x64   -> 0x364
 *     0x364 ShadowModel                0x28   -> 0x38c
 *
 * Typing them absorbed these markers, which were a member's insides:
 *   - 0x128 unk_128      = mdCc_c + 0x18
 *   - 0x130 unk_130      = mdCc_c + 0x20
 *   - 0x134 unk_134      = mdCc_c + 0x24
 *
 * Member NAMES are the ones this header already used -- a rebase should not
 * also rename things its callers spell.
 *
 * SIZE IS THE ROM'S OWN, not a rounded-up field span: `daBmb_c_classInit` calls
 * `fBase_c::operator new(1024)` -- 0x400 -- and stores `_ZTV6BobOmb`,
 * so that literal IS this class's sizeof. The observed fields only span to
 * 0x3f8; the difference is trailing space no source reads.
 *
 * SM64DS RTTI names the implementation daBmb_c. The reconstructed
 * factory daBmb_c_classInit (historical alias
 * BobOmb_Spawn) constructs it for the BOMBHEI
 * registry profile.
 */

#include "dEnemyBase_c.h"
#include "Model.h"
#include "ModelAnim.h"
#include "dCcAc_c.h"
#include "dCcAcPos_c.h"
#include "ShadowModel.h"
#include "TextureTransformer.h"
#include "dBgCh_Actr.h"

struct BobOmb : dEnemyBase_c {
    dCcAc_c           mdCc_c;         /* 0x110 */
    dBgCh_Actr                 mWithMeshClsn;         /* 0x144 */
    ModelAnim                    mModelAnim;            /* 0x300 */
    ShadowModel                  mShadowModel;          /* 0x364 */
    u8  pad_38c[0x4];
    /* Everything from here down was reachable only through the `struct Obj`
       shadow InitResources used to carry, so the generated header never had
       it. mMatrix is a 0x30-byte block copied wholesale from IDENTITY_MATRIX4X3. */
    u32                          unk_390;               /* 0x390 -- InitResources zeroes it; no reader */
    /* A Matrix4x3, copied wholesale from IDENTITY_MATRIX4X3 by InitResources.
       Left as twelve words rather than typed: BobOmb.h is included by files that
       do not pull common.h, and embedding the real type would force it on them. */
    s32                          mMatrix[12];           /* 0x394 */
    s32                          mHomePosX;             /* 0x3c4 */
    s32                          mHomePosY;             /* 0x3c8 */
    s32                          mHomePosZ;             /* 0x3cc */
    u8  pad_3d0[0xc];
    /* Behavior's state selector: it branches on == 5 (skip almost everything),
       == 4 (the egg/Chuckya hand-off) and == 0 (allow the wall bounce). */
    s32                          mState;                /* 0x3dc */
    s32                          unk_3e0;               /* 0x3e0 -- InitResources stores 2; no reader */
    u8  pad_3e4[0x4];
    u16                          unk_3e8;               /* 0x3e8 -- zeroed by InitResources */
    u16                          unk_3ea;               /* 0x3ea -- zeroed by InitResources */
    u16                          unk_3ec;               /* 0x3ec -- InitResources stores 0x2000 */
    u16                          unk_3ee;               /* 0x3ee */
    /* InitResources' last statement: a snapshot of mAngleY taken next to the
       mHomePos* snapshot of the position. */
    u16                          mHomeAngleY;           /* 0x3f0 */
    u8                           unk_3f2;               /* 0x3f2 -- zeroed by InitResources */
    /* Render draws nothing at all while this is 0; InitResources sets it to 1. */
    u8                           mShouldRender;         /* 0x3f3 */
    u8                           unk_3f4;               /* 0x3f4 */
    /* param1 & 7. InitResources switches on it: 2 starts inert (sets the
       collision volume's hit bit and clears mFlags bit 0), 4 starts clear,
       anything else starts live. Behavior reads it again for the egg path. */
    u8                           mVariant;              /* 0x3f5 */
    /* A latch: while non-zero Behavior does nothing but call func_ov102_0214ae1c
       and return. InitResources clears it, and nothing matched sets it, so what
       the latch MEANS is not evidenced -- only that it diverts the whole frame. */
    u8                           unk_3f6;               /* 0x3f6 */
    u8  pad_3f7[0x9];

    /* --- vtable --- */
    virtual ~BobOmb();

    virtual s32   OnYoshiTryEat();         /* slot 18 */
    virtual int   OnTurnIntoEgg(Player &player); /* slot 19 */
    virtual s32   OnAimedAtWithEgg();      /* slot 29 */

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
};

typedef char BobOmb_size_must_be_0x400[sizeof(BobOmb) == 0x400 ? 1 : -1];

#endif /* BOBOMB_H */
