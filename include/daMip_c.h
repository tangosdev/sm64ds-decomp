#ifndef DAMIP_C_H
#define DAMIP_C_H

#include "types.h"

/* Derives from dEnemyBase_c, and TWO INDEPENDENT WITNESSES agree on the layout:
 * the class's own destructor `_ZN7daMip_cD1Ev` destroys each member, and
 * `daMip_c_classInit` constructs the same types at the same offsets before
 * storing `_ZTV7daMip_c`. Everything this header used to restate below
 * 0x110 belongs to dEnemyBase_c and dActor_c and is inherited now.
 *
 * The members close on each other, which is what makes the layout a
 * reading rather than a guess:
 *
 *     0x110 dCcAc_c         0x34    -> 0x144
 *     0x144 dBgCh_Actr               0x1bc   -> 0x300
 *     0x300 ModelAnim                  0x64    -> 0x364
 *     0x368 ShadowModel                0x28    -> 0x390
 *     0x3c0 ShadowModel                0x28    -> 0x3e8
 *
 * Typing them absorbed markers that were their insides:
 *   - unk_308 = ModelAnim.data.modelFile
 *   - unk_30c = ModelAnim.data.materials
 *   - unk_35c = ModelAnim.speed
 *
 * SIZE IS THE ROM'S OWN: `daMip_c_classInit` calls
 * `fBase_c::operator new(1140)` -- 0x474 -- and stores this class's
 * vtable, so that literal IS this class's sizeof.
 *
 * SM64DS proves this class as daMip_c through RTTI, allocation size and
 * vtable identity. The factory and profile spellings below are reconstructed
 * source-style names -- evidence-bounded proposals, not recovered SM64DS
 * symbols.
 *
 * daMip_c_classInit at 0x0212cc2c allocates
 * 0x474 and installs this class's cartridge vtable. It backs the MIP
 * registry profile, whose descriptor at 0x021300d4 is reconstructed as
 * g_profile_MIP.
 */

#include "dEnemyBase_c.h"
#include "ModelAnim.h"
#include "dCcAc_c.h"
#include "ShadowModel.h"
#include "dBgCh_Actr.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

struct daMip_c : dEnemyBase_c {
    dCcAc_c           mdCcAc_c;   /* 0x110 */
    dBgCh_Actr                 mWithMeshClsn;         /* 0x144 */
    ModelAnim                    mModelAnim;            /* 0x300 */
    /* The current animation-state descriptor. Behavior compares it by ADDRESS
       against four file-scope ov085 objects (0213068c / 021306ac / 021306bc /
       021306dc) and reads a pointer-to-member sequence out of it at +0x08, so it
       is a pointer with no recovered type; func_ov085_0212bc78(this, record)
       assigns it. */
    s32                          mState;                /* 0x364 */
    ShadowModel                  mShadowModel1;         /* 0x368 */
    /* 0212bcc8 / 0212bdbc copy data_020a0e68 here then DropShadowRadHeight. */
    s32                          mShadowMtx[12];        /* 0x390 */
    ShadowModel                  mShadowModel2;         /* 0x3c0 */
    u8  pad_3e8[0x34];
    /* Idle/talk/save step: StateIdleMain cycles 0..2; StateSaveTalkMain walks
       0,1,2; several inits zero it. */
    s32                          mActionStep;           /* 0x41c */
    s32                          mColorVariant;         /* 0x420 */
    /* HorzAngle target; ApproachAngle reads it (StateSaveTalkMain / StateIdleMain). */
    s16                          mTargetAngY;           /* 0x424 */
    u8                           unk_426;               /* 0x426 -- set to 2 the frame Yoshi eats it;
                                                            read only as `!= 0`, to force the idle
                                                            state and zero mHorzSpeed */
    /* The talk sequence: 0 offers the message, 1 waits for the player's talk
       state to end, 2 is done. Behavior walks it in exactly that order. */
    u8                           mTalkState;            /* 0x427 */
    /* Latches the rabbit out of the level: while it is 1 Behavior returns
       immediately and Render draws nothing. */
    u8                           mIsDisabled;           /* 0x428 */
    /* The rare glowing rabbit. InitResources sets it when mColorVariant lands on
       5; Behavior then spawns the glow particle every frame and picks the
       "you found a glowing one" message instead of the ordinary line. */
    u8                           mIsGlowing;            /* 0x429 */
    /* Counted up while dEnemyBase_c's mEatenByYoshi reads 1 (inside the Yoshi-eat
       branch) and reset together with it once it passes 0x96. */
    u8                           mEatenTimer;           /* 0x42a */
    u8  pad_42b[0x1];
    /* StateIdleInit snapshots pos here; StateIdleMain HorzAngle's to it. */
    s32                          mIdlePosX;             /* 0x42c */
    s32                          mIdlePosY;             /* 0x430 */
    s32                          mIdlePosZ;             /* 0x434 */
    s32                          mPathId;               /* 0x438 -- param1 & 0xff, handed to PathPtr::FromID */
    s32                          mRabbitId;             /* 0x43c */
    s32                          mCharacterId;          /* 0x440 */
    s32                          mNumPathNodes;         /* 0x444 -- PathPtr::NumNodes() */
    s32                          mPathNodeIndex;        /* 0x448 -- the index handed to PathPtr::GetNode */
    /* StateFleeInit: +1 or -1 along the path. */
    s32                          mPathDir;              /* 0x44c */
    u8  pad_450[0xc];
    /* Assigned ClosestPlayer(); ShowMessage / GetTalkState / DropActor. */
    Player                      *mTalkingPlayer;        /* 0x45c */
    /* StateSaveTalkMain / StateCaughtMain store the player they turn toward. */
    Player                      *mSaveTalkPlayer;       /* 0x460 */
    /* TestWaterBelow caches the water-detect clsn Y. */
    s32                          mFloorY;               /* 0x464 */
    /* (mColorVariant << 1) + the material record's own +0x20 word. Render writes
       it back into +0x20 of EVERY material of the model, which is how the six
       rabbit colours come out of one model file. */
    s32                          mMaterialColor;        /* 0x468 */
    /* StateFleeMain / StateIdleMain splash-or-dust particle handle. */
    s32                          mDustParticle;         /* 0x46c */
    /* The glow particle's handle: Behavior passes the old value straight back
       into Particle::System::New and stores the result. Glowing rabbits only. */
    s32                          mGlowParticle;         /* 0x470 */

    /* --- vtable --- */
    virtual ~daMip_c();

    virtual s32   OnYoshiTryEat();         /* slot 18 */

    int Behavior();
    int CleanupResources();
    int InitResources();
    void OnPendingDestroy();
    int Render();

    /* --- helpers ---
       Member-ness is ROM-proven for SetState and for every State* below: each
       is the target of an 8-byte {fnptr, 0} pointer-to-member-function constant
       in ov085 .data at 0x0213003c..0x021300bc, and such a record can point at
       nothing but a member of this class. The other six take the object in r0
       and address it this-relatively throughout; whether the original source
       spelled them members or file-local functions taking a daMip_c* is NOT
       recovered, and the two forms are indistinguishable in the cartridge's
       bytes. Every NAME here is coined -- see symbols/actor_renames.tsv for the
       per-name evidence. */
    int  TestWaterBelow();
    void UpdateGrab();
    int  SetState(void *record);
    void UpdateMatrixAndShadow();
    void UpdateCarriedMatrix();
    void UpdateMirrorShadow();
    void RenderMirrorImage();

    /* --- the eight animation states ---
       Each pair is one 16-byte .bss record = {Init, Main}; the pairing and the
       order are proven by __sinit_ov085_0212f5ec, which builds all eight. */
    int  StateStartleInit();               /* record 0x0213066c */
    int  StateStartleMain();
    int  StateFleeInit();                  /* record 0x0213067c */
    int  StateFleeMain();
    int  StateSaveTalkInit();              /* record 0x0213068c */
    int  StateSaveTalkMain();
    int  StateRestInit();                  /* record 0x0213069c */
    int  StateRestMain();
    int  StateCaughtInit();                /* record 0x021306ac */
    int  StateCaughtMain();
    int  StateReleasedInit();              /* record 0x021306bc */
    int  StateReleasedMain();
    int  StateIdleInit();                  /* record 0x021306cc */
    int  StateIdleMain();
    int  StateTalkInit();                  /* record 0x021306dc */
    int  StateTalkMain();

    static void *operator new(unsigned long size) {
        return _ZN7fBase_cnwEj((unsigned)size);
    }
};

typedef char daMip_c_size_must_be_0x474[sizeof(daMip_c) == 0x474 ? 1 : -1];

/* ~daMip_c, the key function, owns the compiler-emitted definition of this
 * vtable. The factory is `return new daMip_c()`; the leaf size_t operator new
 * forwards to `_ZN7fBase_cnwEj` until #2570's fBase overload lands. */
extern int _ZTV7daMip_c[];

#endif /* DAMIP_C_H */
