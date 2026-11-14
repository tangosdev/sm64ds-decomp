/* class daIwante_c : dBgActor_c. Real C++ form.
 *
 * Base and size from the factory (reconstructed name daIwante_c_classInit,
 * now `new daIwante_c()` at the bottom of src/game/actors/d_a_iwante.cpp):
 * fBase_c::operator new(0x874), then
 * dBgActor_c::dBgActor_c(), then the _ZTV10daIwante_c store. Two
 * vtable stores in the destructor -- own, then dBgActor_c's -- confirming a
 * DIRECT dBgActor_c child, no intermediate. The apparent third store some
 * tooling flagged is `extern int _ZN7Vector3D1Ev[];` passed as a callback
 * pointer to `__cxa_vec_cleanup` (see the D1 body) -- a literal-pool FUNCTION
 * POINTER argument, not a vptr store; it never writes to `this`.
 *
 * dBgActor_c ends at 0x320. Every member below closes exactly on the next
 * (confirmed against the factory and the D1 destructor, which construct and
 * destroy each in this order):
 *
 *     dCcAcPos_c         0x320 + 0x40 = 0x360
 *     dExtBlendAnmModel_c     0x360 + 0x70 = 0x3d0
 *     dExtSimpleModel_c              0x3d0 + 0x50 = 0x420
 *     dExtShadowModel_c  0x420 + 0x28 = 0x448
 *     dExtAnmTexPat_c    0x448 + 0x14 = 0x45c
 *
 * 0x45c..0x4d6 is a run of individually evidenced scalars. At 0x4dc,
 * Vector3[0x14] (0xc == sizeof(Vector3)) -- destroyed with
 * __cxa_vec_cleanup(ptr, 0x14, 0xc, _ZN7Vector3D1Ev), same evidence shape as
 * include/daMoray_c.h's mStarUniqueID -- ends at 0x5cc.
 *
 * THE CLASS NOW CLOSES ON ITS OWN SIZE. Reading Behavior and InitResources as
 * named members (see notes/bgobject-provenance.md) turned every remaining pad
 * in this class into an evidenced field: the 0xa8 bytes after the Vector3
 * array are two 0x14-entry particle-handle arrays plus the star id/tracked
 * pair the ROM writes at 0x672/0x673, and the "unused tail" at 0x83c is the
 * Matrix4x3 InitResources passes to dBgW_KcMbg::SetFile followed by the two
 * uniqueIDs of the hands it spawns. 0x870 + 4 = 0x874, which is exactly the
 * literal the factory passes to operator new -- the size is now
 * corroborated by the field span rather than merely asserted over it.
 *
 * 0x674 is a second, class-owned dBgW_KcMbg (named by _ZN10dBgW_KcMbgD1Ev in
 * the destructor), distinct from dBgActor_c's own at 0x124.
 *
 * SM64DS RTTI names the implementation daIwante_c. The reconstructed
 * factory daIwante_c_classInit (historical alias Eyerok_Spawn) constructs it
 * for the IWANTE registry profile.
 */
#ifndef DAIWANTE_C_H
#define DAIWANTE_C_H
#include "types.h"
#include "dExtSimpleModel_c.h"
#include "dBgW_KcMbg.h"
#include "dBgActor_c.h"
#include "dCcAcPos_c.h"
#include "dExtBlendAnmModel_c.h"
#include "dExtShadowModel_c.h"
#include "dExtAnmTexPat_c.h"

/* Defined in d_a_iwante.cpp: the enter/run pointer-to-member pair. */
struct EyerokState;

struct daIwante_c : dBgActor_c {
    dCcAcPos_c mdCcAcPos_c;                                 /* 0x320 */
    dExtBlendAnmModel_c mBlendAnmModel;                     /* 0x360 */
    dExtSimpleModel_c mModel2;                              /* 0x3d0 */
    dExtShadowModel_c mShadowModel;                         /* 0x420 */
    dExtAnmTexPat_c mTextureSequence;                       /* 0x448 */
    /* Drop-shadow matrix passed to DropShadowRadHeight. */
    Matrix4x3 mShadowMtx; /* 0x45c */
    /* Behavior loads this word and calls through the pointer-to-member at
       +8 of what it points at, and compares it against &data_ov066_0211b07c --
       so it is a pointer to the current state descriptor, not a byte.
       SetState is what installs one. */
    EyerokState *mState;                                    /* 0x48c */
    /* The player locked for the talk state's dialogue (BodyTalkExecute):
       the message before the fight and the one after it. */
    daPly_c *mTalkPlayer; /* 0x490 */
    /* Per-state scratch; most enter handlers clear both (the dormant enter
       handler does nothing). mStateWork0: attacking-hand toggle (02118cdc),
       stomp count (pattern 8), 20-frame stand-down counter (pattern 5),
       rise-started flag. mStateWork1: ShowMessage latch (0211903c),
       hit-volume-armed flag (BeginVulnerable). */
    s32 mStateWork0;                                        /* 0x494 */
    s32 mStateWork1;                                        /* 0x498 */
    s32 mPartIdx;                                           /* 0x49c */
    /* Step within the current state. Most enter handlers reset it to 0. */
    s32 mSubState;                                          /* 0x4a0 */
    /* The part's rest position: InitResources seeds it from the actor position
       and then offsets it (a hand goes -+0x31f000 in X, -0x32000 in Z), and
       Behavior re-derives its Y from mSpawnPosY every frame. */
    s32 mRestPosX;                                          /* 0x4a4 */
    s32 mRestPosY;                                          /* 0x4a8 */
    s32 mRestPosZ;                                          /* 0x4ac */
    /* The unmoved spawn position, snapshotted only on the two hands. */
    s32 mSpawnPosX;                                         /* 0x4b0 */
    s32 mSpawnPosY;                                         /* 0x4b4 */
    s32 mSpawnPosZ;                                         /* 0x4b8 */
    /* The point a hand is currently moving to. The state handlers fill it
       (from the closest player's position, or a fixed spot) and then walk
       mPosX/Z toward it with Vec3_ApproachHorz / ApproachLinear. */
    Vector3 mTargetPos; /* 0x4bc */
    /* Per-frame step handed to ApproachLinear(mHorzSpeed, 0x258000, step):
       the sideways-run states bump it by 0x1a or 0x130 each frame while it is
       below 0x2710. */
    s32 mHorzAccelStep;                                     /* 0x4c8 */
    u8  pad_4cc[0x4];
    /* Both are counted down once a frame by DecIfAbove0_Short, which takes a
       u16 * -- 0x4d0 was declared u8 + 1 byte of padding until that was read.
       mTimer1 is the per-state countdown the handlers arm and test for zero;
       mTimer2 is armed (0x64 or 0x1e frames) by InitResources and before the
       body enters its talk or decision state; both of those wait for it to
       reach 0. */
    u16 mTimer1;                                            /* 0x4d0 */
    u16 mTimer2;                                            /* 0x4d2 */
    /* Dust-burst frame counter: 0 = idle. A fire hit (0x40000 in the hit mask)
       sets it to 1; Behavior then bumps it every frame, emits dust every other
       frame, and clears the 0x14 dust slots once it passes 0x26. */
    u16 mDustCounter;                                       /* 0x4d4 */
    u8  pad_4d6[0x2];
    /* A hand's remaining hits: InitResources sets 3 on the two hands, the hit
       check (CheckAttacked) subtracts per hit and a hand with 0 or fewer
       left is defeated. Nothing in d_a_iwante.cpp writes it for the main
       instance. */
    s8  mHitPoints;                                         /* 0x4d8 */
    /* Picks made by patterns 5..7 since the last pattern 8 or 9 (it starts
       at 0); the decision state compares it with
       data_ov066_0211abe4 + 3. */
    u8  mPickCount;                                         /* 0x4d9 */
    u8  pad_4da[0x2];
    /* The ROM destroys this with __cxa_vec_cleanup(this + 0x4dc, 0x14, 0xc,
       _ZN7Vector3D1Ev) -- 0x14 elements, 0xc == sizeof(Vector3), same
       evidence shape as include/daMoray_c.h's mStarUniqueID. The three
       loops in d_a_iwante.cpp walk it with a stepping char * and read
       mDustPos[0] through it, so the count is trusted from the destructor
       call and the 0x14-iteration loops, not from any indexed access. */
    Vector3 mDustPos[0x14];                                 /* 0x4dc */
    /* One recycled Particle::System handle per mDustPos slot, per effect id.
       Behavior reissues both for each slot with a nonzero position
       while mDustCounter is nonzero. Was pad_5cc. */
    u32 mDustParticle1[0x14];                               /* 0x5cc */
    u32 mDustParticle2[0x14];                               /* 0x61c */
    /* Texture-pattern swap timer and phase (UpdateTexturePattern). When the
       timer reaches 0: phase 0 -> switch to the ae2c / ae9c pattern for
       0x32..0x50 frames (random); phase 1 -> back to ae3c / aebc for 8
       frames; the phase flips each time. Which pattern is which is not
       identified. */
    u16 mTexTimer;                                          /* 0x66c */
    u8  mTexPhase;                                          /* 0x66e */
    u8  pad_66f[0x3];
    u8  mStarId;                                            /* 0x672 */
    s8  mStarTracked;                                       /* 0x673 */
    dBgW_KcMbg mMeshCollider2;                              /* 0x674 -- this class's own, not dBgActor_c's */
    /* NOT unused tail. InitResources passes `this + 0x83c` as the Matrix4x3 &
       argument of dBgW_KcMbg::SetFile on every path, and writes the two words
       after it with the uniqueIDs of the two hands it spawns. 0x83c + 0x30 =
       0x86c, and 0x870 + 4 = 0x874, the literal the factory passes to
       operator new -- so the class now closes on its own size. */
    Matrix4x3 mClsnMat2;                                    /* 0x83c */
    s32 mHandUniqueID1;                                     /* 0x86c */
    s32 mHandUniqueID2;                                     /* 0x870 */

    /* --- vtable --- */
    virtual ~daIwante_c();

    int Behavior();
    int CleanupResources();
    int InitResources();
    void OnPendingDestroy();                 /* slot 12 -- empty body in the ROM */
    int Render();

    /* Slot 29, dActor_c's egg-aim callback (include/dActor_c.h). Attributed
       by the vtable: _ZTV10daIwante_c + 4*29 = 0x0211ad64 + 0x74 = 0x0211ade8;
       config/arm9/overlays/ov066/relocs.txt confirms 0x0211ade8 -> 0x0211a2dc. */
    int OnAimedAtWithEgg();  /* slot 29 */

    /* The state handlers the descriptors' pointer-to-member pairs name
       (enter at +0, run at +8), plus the helpers they call. The original
       names are not recovered; the addresses stand in. */
    int CheckAttacked();             /* hand hit check (hurt/defeat) */
    void SetRestHitVolume();         /* arm a hand's hit volume */
    void SetPoundHitVolume();        /* disarm a hand's hit volume */
    void UpdateTexturePattern();     /* texture-pattern swap tick */
    void BeginVulnerable();          /* arm hit volume once anim started */
    void EndVulnerable();            /* hand becomes vulnerable */
    void SetLoweredPose();           /* common state cleanup on exit */
    int WaitForHandsReady();         /* body: wait for both hands to report in */
    int FollowPhase();               /* hand: follow the phase into a state */
    int GetPlayerDepthBand();        /* closest player's depth band (mPosZ) */
    void PoundGround(int strength);  /* landing dust + shake */
    int ClampToArena();              /* keep a hand inside the arena */
    int HandDefeatExecute();         /* defeat run (hand) */
    int HandDefeatEnter();           /* defeat enter (hand) */
    int HandPattern9Execute();       /* pattern 9 run (hand) */
    int HandPattern9Enter();         /* pattern 9 enter (hand) */
    int HandPattern8Execute();       /* pattern 8 run (hand) */
    int HandPattern8Enter();         /* pattern 8 enter (hand) */
    int HandPattern7Execute();       /* pattern 7 run (hand) */
    int HandPattern7Enter();         /* pattern 7 enter (hand) */
    int HandPattern6Execute();       /* pattern 6 run (hand) */
    int HandPattern6Enter();         /* pattern 6 enter (hand) */
    int HandPattern5Execute();       /* pattern 5 run (hand) */
    int HandPattern5Enter();         /* pattern 5 enter (hand) */
    int HandPattern4Execute();       /* pattern 4 run (hand) */
    int HandPattern4Enter();         /* pattern 4 enter (hand) */
    int HandWaitingExecute();        /* waiting run (hand) */
    int HandWaitingEnter();          /* waiting enter (hand) */
    int HandRiseExecute();           /* rise run (hand) */
    int HandRiseEnter();             /* rise enter (hand) */
    int BodyPatternRunningExecute(); /* pattern-running run (body) */
    int BodyPatternRunningEnter();   /* pattern-running enter (body) */
    s32 BodyPattern9Execute();       /* pattern 9 run (body) */
    int BodyPattern9Enter();         /* pattern 9 enter (body) */
    int BodyPattern8Execute();       /* pattern 8 run (body) */
    int BodyPattern8Enter();         /* pattern 8 enter (body) */
    s32 BodyPattern7Execute();       /* pattern 7 run (body) */
    int BodyPattern7Enter();         /* pattern 7 enter (body) */
    s32 BodyPattern6Execute();       /* pattern 6 run (body) */
    int BodyPattern6Enter();         /* pattern 6 enter (body) */
    s32 BodyPattern5Execute();       /* pattern 5 run (body) */
    int BodyPattern5Enter();         /* pattern 5 enter (body) */
    int BodyPattern4Execute();       /* pattern 4 run (body) */
    int BodyPattern4Enter();         /* pattern 4 enter (body) */
    int BodyDecisionExecute();       /* decision run (body) */
    int BodyDecisionEnter();         /* decision enter (body) */
    int BodyTalkExecute();           /* talk run (body) */
    int BodyTalkEnter();             /* talk enter (body) */
    int BodyDormantExecute();        /* dormant run (body) */
    int BodyDormantEnter();          /* dormant enter (body; does nothing) */
    int SetState(void *pv);          /* install a state descriptor */
    void UpdateCollisionMatrix();    /* refresh the collision matrix */
    void UpdateModelMatrices();      /* refresh the model matrices */

    /* mMeshCollider2's stood-on handler and the dBgW veneer that reaches it. */
    void OnStoodOn(dActor_c *other);
    static void StoodOnCallback(dBgW *collider, dActor_c *owner, dActor_c *other);
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char Eyerok_size_must_be_0x874[sizeof(daIwante_c) == 0x874 ? 1 : -1];
#endif

#endif
