#ifndef RABBIT_H
#define RABBIT_H

#include "types.h"

/* Derives from dEnemyBase_c, and TWO INDEPENDENT WITNESSES agree on the layout:
 * the class's own destructor `_ZN6RabbitD1Ev` destroys each member, and
 * `Rabbit_Spawn` constructs the same types at the same offsets before
 * storing `_ZTV6Rabbit`. Everything this header used to restate below
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
 * SIZE IS THE ROM'S OWN: `Rabbit_Spawn` calls
 * `fBase_c::operator new(1140)` -- 0x474 -- and stores this class's
 * vtable, so that literal IS this class's sizeof.
 */

#include "dEnemyBase_c.h"
#include "ModelAnim.h"
#include "dCcAc_c.h"
#include "ShadowModel.h"
#include "dBgCh_Actr.h"

struct Rabbit : dEnemyBase_c {
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
    u8  pad_390[0x30];
    ShadowModel                  mShadowModel2;         /* 0x3c0 */
    u8  pad_3e8[0x38];
    s32                          mColorVariant;         /* 0x420 */
    u8  pad_424[0x2];
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
    u8  pad_42b[0xd];
    s32                          mPathId;               /* 0x438 -- param1 & 0xff, handed to PathPtr::FromID */
    s32                          mRabbitId;             /* 0x43c */
    s32                          mCharacterId;          /* 0x440 */
    s32                          mNumPathNodes;         /* 0x444 -- PathPtr::NumNodes() */
    s32                          mPathNodeIndex;        /* 0x448 -- the index handed to PathPtr::GetNode */
    u8  pad_44c[0x10];
    /* A Player *. Assigned dActor_c::ClosestPlayer() and then passed to
       Player::ShowMessage, Player::GetTalkState and Player::DropActor. */
    s32                          mTalkingPlayer;        /* 0x45c */
    u8  pad_460[0x8];
    /* (mColorVariant << 1) + the material record's own +0x20 word. Render writes
       it back into +0x20 of EVERY material of the model, which is how the six
       rabbit colours come out of one model file. */
    s32                          mMaterialColor;        /* 0x468 */
    u8  pad_46c[0x4];
    /* The glow particle's handle: Behavior passes the old value straight back
       into Particle::System::New and stores the result. Glowing rabbits only. */
    s32                          mGlowParticle;         /* 0x470 */

    /* --- vtable --- */
    virtual ~Rabbit();

    virtual s32   OnYoshiTryEat();         /* slot 18 */

    int Behavior();
    int CleanupResources();
    int InitResources();
    void OnPendingDestroy();
    int Render();
};

typedef char Rabbit_size_must_be_0x474[sizeof(Rabbit) == 0x474 ? 1 : -1];

#endif /* RABBIT_H */
