#ifndef DABMB_C_H
#define DABMB_C_H

#include "types.h"

/* Derives from dEnemyBase_c, on the evidence of its own destructor: `_ZN7daBmb_cD1Ev`
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
 * `fBase_c::operator new(1024)` -- 0x400 -- and stores `_ZTV7daBmb_c`,
 * so that literal IS this class's sizeof. Particle handles at 0x3f8 / 0x3fc
 * are written and read (0214b53c).
 *
 * THE NAME IS THE CARTRIDGE'S OWN. ov102 0x0214e4fc holds the bytes
 * "7daBmb_c\0" -- the length-prefixed mangled type name -- and _ZTI7daBmb_c at
 * 0x0214e508 points its +4 word back at that string, while the vtable's -4
 * header word points at the _ZTI. The class was carried here under the coined
 * name BobOmb, which occurs nowhere in the cartridge; that spelling is gone and
 * every member now mangles as _ZN7daBmb_c*. The reconstructed factory
 * daBmb_c_classInit (historical alias BobOmb_Spawn) constructs it for the
 * BOMBHEI registry profile.
 */

#include "dEnemyBase_c.h"
#include "Model.h"
#include "ModelAnim.h"
#include "dCcAc_c.h"
#include "dCcAcPos_c.h"
#include "ShadowModel.h"
#include "TextureTransformer.h"
#include "dBgCh_Actr.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

struct daBmb_c : dEnemyBase_c {
    dCcAc_c           mdCc_c;         /* 0x110 */
    dBgCh_Actr                 mWithMeshClsn;         /* 0x144 */
    ModelAnim                    mModelAnim;            /* 0x300 */
    ShadowModel                  mShadowModel;          /* 0x364 */
    /* 0214b988 stores ClosestPlayer here after the notice-angle test;
       0214ad14 stores it unconditionally. Arm 0 chases this pointer. */
    Player                      *mChasePlayer;          /* 0x38c */
    /* The Player carrying this Bob-omb, or 0. 0214b53c passes it to
       UpdateCarry and calls IsFrontSliding / LostGrabbedObject; 0214ae1c
       calls DropActor and Hurt, then clears it; 0214b3f0 sets it and
       0214b3b8 clears it. InitResources zeroes it. */
    Player                      *mCarrier;              /* 0x390 */
    /* Twelve words, not a Matrix4x3: this header is included by files that
       do not pull common.h. InitResources copies IDENTITY_MATRIX4X3 here.
       0214b444 writes the translation at [9..11] before the shadow drop. */
    s32                          mMatrix[12];           /* 0x394 */
    s32                          mHomePosX;             /* 0x3c4 */
    s32                          mHomePosY;             /* 0x3c8 */
    s32                          mHomePosZ;             /* 0x3cc */
    /* UpdateCarry offset. Zeroed when the Bob-omb is not in a player's hand. */
    s32                          mCarryOff[3];          /* 0x3d0 */
    /* Behavior skips the body at 5, the egg hand-off is 4, the wall bounce
       is allowed only at 0. 0214b03c dispatches arms 0..5. */
    s32                          mState;                /* 0x3dc */
    /* Hurt damage. InitResources stores 2. ov078/daBombking_c writes 0. */
    s32                          unk_3e0;               /* 0x3e0 */
    /* Sound::PlayLong handle for the fuse tick. */
    u32                          mFuseSound;            /* 0x3e4 */
    u16                          mTurnTimer;            /* 0x3e8 */
    /* Fuse countdown. 0214b384 only shortens it; 0214b248 explodes at 1. */
    u16                          mFuse;                 /* 0x3ea */
    /* Half-angle window. 0214b988 will not chase a player outside it.
       InitResources stores 0x2000. */
    u16                          mNoticeAngle;          /* 0x3ec */
    /* State1 walks mPrevAngleY toward this. beb4 adds a random kick when
       the Bob-omb is turning with nobody to chase. */
    u16                          mTargetAngY;           /* 0x3ee */
    /* Snapshot of mAngleY beside the mHomePos* snapshot. */
    u16                          mHomeAngleY;           /* 0x3f0 */
    u8                           mTurnCount;            /* 0x3f2 */
    /* Render draws nothing while this is 0. InitResources sets it to 1. */
    u8                           mShouldRender;         /* 0x3f3 */
    u8                           unk_3f4;               /* 0x3f4 */
    /* param1 & 7. 2 starts inert, 4 starts clear, anything else starts live. */
    u8                           mVariant;              /* 0x3f5 */
    /* Non-zero: Behavior detonates and returns. ov078/daBombking_c sets 1. */
    u8                           unk_3f6;               /* 0x3f6 */
    u8                           pad_3f7;               /* 0x3f7 */
    u32                          mCarryParticle;        /* 0x3f8 -- effect 0x13 while carried */
    u32                          mFuseParticle;         /* 0x3fc -- effect 0x19 while the fuse is lit */

    /* --- vtable --- */

    /* INLINE, AND DECLARED FIRST. The cartridge puts D1 at 0x0214a96c below D0
       at 0x0214a9b4 and carries no D2 anywhere, which is what mwccarm 2004/b56
       emits for an inline in-class destructor; the out-of-line form emits
       D2/D0/D1 in the wrong order plus a homeless D2. The typed member list
       above makes the empty body own the ShadowModel, ModelAnim, dBgCh_Actr and
       dCcAc_c teardowns and the chain into _ZN12dEnemyBase_cD2Ev.

       With the destructor inline, OnYoshiTryEat becomes the first out-of-line
       virtual this class declares -- the key function -- so the vtable and the
       RTTI group land in the translation unit that defines it,
       src/actors/daBmb_c.cpp. */
    virtual ~daBmb_c() {}

    virtual s32   OnYoshiTryEat();         /* slot 18 -- key function */
    virtual void  OnTurnIntoEgg(Player &player); /* slot 19 */
    virtual s32   OnAimedAtWithEgg();      /* slot 29 */

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();

    /* Four of the six arms of the mState state machine.  The NUMBER is what the
       cartridge proves: func_ov102_0214b03c switches on mState (+0x3dc) and
       dispatches exactly six bodies for 0..5, one each, and nothing else reads the
       field to choose between them.  State1/3/4/5 are coined -- ov102 carries these
       addresses and no identifier -- so they claim the index and nothing more; what
       each arm does is written at its body.  Arm 0 stays a free function
       (func_ov102_0214bf64): its body casts to Bmb_Bf64Obj, a different object.
       func_ov102_0214bd90 and func_ov102_0214b03c are methods; the address is the
       method name. */
    void State1();
    void State3();
    int  State4();
    void State5();
    void func_ov102_0214bd90();
    void func_ov102_0214b03c();

    /* Leaf adapter until fBase_c::operator new(unsigned long) lands (#2570).
       `return new daBmb_c()` then routes through the retail allocator. */
    static void *operator new(size_t size) {
        return _ZN7fBase_cnwEj((unsigned)size);
    }
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daBmb_c_size_must_be_0x400[sizeof(daBmb_c) == 0x400 ? 1 : -1];
#endif

#endif /* DABMB_C_H */
