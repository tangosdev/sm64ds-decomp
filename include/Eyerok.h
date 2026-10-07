/* class Eyerok : dBgActor_c. Real C++ form.
 *
 * Base and size from the factory (src/daIwante_c_classInit.cpp): fBase_c::operator
 * new(0x874), then dBgActor_c::dBgActor_c(), then stores _ZTV6Eyerok. Two
 * vtable stores in the destructor -- own, then dBgActor_c's -- confirming a
 * DIRECT dBgActor_c child, no intermediate. The apparent third store some
 * tooling flagged is `extern int _ZN7Vector3D1Ev[];` passed as a callback
 * pointer to `__cxa_vec_cleanup` (see the D1 body) -- a literal-pool FUNCTION
 * POINTER argument, not a vptr store; it never writes to `this`.
 *
 * dBgActor_c ends at 0x320. Every member below closes exactly on the next
 * (confirmed against src/daIwante_c_classInit.cpp and src/_ZN6EyerokD1Ev.c, which
 * construct/destroy each in this order):
 *
 *     dCcAcPos_c  0x320 + 0x40 = 0x360
 *     BlendModelAnim             0x360 + 0x70 = 0x3d0
 *     Model                      0x3d0 + 0x50 = 0x420
 *     dExtShadowModel_c                0x420 + 0x28 = 0x448
 *     TextureSequence            0x448 + 0x14 = 0x45c
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
 * literal src/daIwante_c_classInit.cpp passes to operator new -- the size is now
 * corroborated by the field span rather than merely asserted over it.
 *
 * 0x674 is a second, class-owned dBgW_KcMbg (named by _ZN10dBgW_KcMbgD1Ev in
 * the destructor), distinct from dBgActor_c's own at 0x124.
 *
 * SM64DS RTTI names the implementation daIwante_c. The reconstructed
 * factory daIwante_c_classInit (historical alias
 * Eyerok_Spawn) constructs it for the IWANTE
 * registry profile.
 */
#ifndef EYEROK_H
#define EYEROK_H
#include "types.h"
#include "Model.h"
#include "dBgW_KcMbg.h"

#ifdef __cplusplus

#include "dBgActor_c.h"
#include "dCcAcPos_c.h"
#include "BlendModelAnim.h"
#include "dExtShadowModel_c.h"
#include "TextureSequence.h"

struct Eyerok : dBgActor_c {
    dCcAcPos_c mdCcAcPos_c;  /* 0x320 */
    BlendModelAnim mBlendModelAnim;                        /* 0x360 */
    Model mModel2;                                         /* 0x3d0 */
    dExtShadowModel_c mShadowModel;                               /* 0x420 */
    TextureSequence mTextureSequence;                       /* 0x448 */
    /* Drop-shadow matrix, flat words. A Matrix4x3 member would run another
       ~Vector3 from ~Eyerok. */
    s32 mShadowMtx[12];                                     /* 0x45c */
    /* Behavior loads this word and calls through the pointer-to-member at
       +8 of what it points at, and compares it against &data_ov066_0211b07c --
       so it is a pointer to the current state descriptor, not a byte.
       func_ov066_02119454 is what installs one. */
    void *mState;                                           /* 0x48c */
    /* The player locked for the talk state's dialogue (func_ov066_0211903c):
       the message before the fight and the one after it. */
    Player *mTalkPlayer;                                    /* 0x490 */
    /* Per-state scratch; most enter handlers clear both (the dormant enter
       handler does nothing). mStateWork0: attacking-hand toggle (02118cdc),
       stomp count (pattern 8), 20-frame stand-down counter (pattern 5),
       rise-started flag. mStateWork1: ShowMessage latch (0211903c),
       hit-volume-armed flag (func_ov066_021164ec). */
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
    s32 mTargetPosX;                                        /* 0x4bc */
    s32 mTargetPosY;                                        /* 0x4c0 */
    s32 mTargetPosZ;                                        /* 0x4c4 */
    /* Per-frame step handed to ApproachLinear(&mHorzSpeed, 0x258000, step):
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
       check (func_ov066_0211603c) subtracts per hit and a hand with 0 or fewer
       left is defeated. Nothing in Eyerok.cpp writes it for the main
       instance. */
    s8  mHitPoints;                                         /* 0x4d8 */
    /* Picks made by patterns 5..7 since the last pattern 8 or 9 (it starts
       at 0); the decision state compares it with
       data_ov066_0211abe4 + 3. */
    u8  mPickCount;                                         /* 0x4d9 */
    u8  pad_4da[0x2];
    /* The ROM destroys this with __cxa_vec_cleanup(this + 0x4dc, 0x14, 0xc,
       _ZN7Vector3D1Ev) -- 0x14 elements, 0xc == sizeof(Vector3), same
       evidence shape as include/daMoray_c.h's mStarUniqueID. Only raw
       `this + 0x4dc` / `+ 0x4e0` / `+ 0x4e4` offsets are read elsewhere
       (one Vector3), so the count is trusted from the destructor call, not
       from any indexed access. */
    Vector3 mDustPos[0x14];                                 /* 0x4dc */
    /* One recycled Particle::System handle per mDustPos slot, per effect id.
       Behavior reissues both for each slot with a nonzero position
       while mDustCounter is nonzero. Was pad_5cc. */
    u32 mDustParticle1[0x14];                               /* 0x5cc */
    u32 mDustParticle2[0x14];                               /* 0x61c */
    /* Texture-pattern swap timer and phase (func_ov066_02116390). When the
       timer reaches 0: phase 0 -> switch to the ae2c / ae9c pattern for
       0x32..0x50 frames (random); phase 1 -> back to ae3c / aebc for 8
       frames; the phase flips each time. Which pattern is which is not
       identified. */
    u16 mTexTimer;                                          /* 0x66c */
    u8  mTexPhase;                                          /* 0x66e */
    u8  pad_66f[0x3];
    u8  mStarId;                                            /* 0x672 */
    u8  mStarTracked;                                       /* 0x673 */
    dBgW_KcMbg mMeshCollider2;                              /* 0x674 -- this class's own, not dBgActor_c's */
    /* NOT unused tail. InitResources passes `this + 0x83c` as the Matrix4x3 &
       argument of dBgW_KcMbg::SetFile on every path, and writes the two words
       after it with the uniqueIDs of the two hands it spawns. 0x83c + 0x30 =
       0x86c, and 0x870 + 4 = 0x874, the literal src/daIwante_c_classInit.cpp passes to
       operator new -- so the class now closes on its own size. */
    Matrix4x3 mClsnMat2;                                    /* 0x83c */
    s32 mHandUniqueID1;                                     /* 0x86c */
    s32 mHandUniqueID2;                                     /* 0x870 */

    /* --- vtable --- */
    virtual ~Eyerok();

    int Behavior();
    int CleanupResources();
    int InitResources();
    void OnPendingDestroy();                 /* slot 12 -- empty body in the ROM */
    int Render();

    /* Slot 29, dActor_c's egg-aim callback (include/dActor_c.h). Attributed
       by the vtable: _ZTV6Eyerok + 4*29 = 0x0211ad64 + 0x74 = 0x0211ade8;
       config/arm9/overlays/ov066/relocs.txt confirms 0x0211ade8 -> 0x0211a2dc. */
    int OnAimedAtWithEgg();  /* slot 29 */

    /* The state handlers the descriptors' pointer-to-member pairs name
       (enter at +0, run at +8), plus the helpers they call. The original
       names are not recovered; the addresses stand in. */
    int  func_ov066_0211603c();    /* hand hit check (hurt/defeat) */
    void func_ov066_021162e8();    /* arm a hand's hit volume */
    void func_ov066_0211632c();    /* disarm a hand's hit volume */
    void func_ov066_02116390();    /* texture-pattern swap tick */
    void func_ov066_021164ec();    /* arm hit volume once anim started */
    void func_ov066_021165cc();    /* hand becomes vulnerable */
    void func_ov066_021166c8();    /* common state cleanup on exit */
    int  func_ov066_021168b0();    /* alive-hand mask query */
    int  func_ov066_021168ec();    /* body's current phase */
    int  func_ov066_02116a68();    /* closest-player X (arena-fixed) */
    void func_ov066_02116ac4(int strength); /* landing dust + shake */
    int  func_ov066_02116b78();    /* keep a hand inside the arena */
    int  func_ov066_02116c6c();    /* defeat run */
    int  func_ov066_02116d14();    /* defeat enter */
    int  func_ov066_02116db0();    /* pattern 9 run */
    int  func_ov066_02117190();    /* pattern 9 enter */
    int  func_ov066_021171b0();    /* pattern 8 run */
    int  func_ov066_021175bc();    /* pattern 8 enter */
    int  func_ov066_021175e8();    /* pattern 7 run */
    int  func_ov066_02117bd0();    /* pattern 7 enter */
    int  func_ov066_02117bf0();    /* pattern 6 run */
    int  func_ov066_02118168();    /* pattern 6 enter */
    int  func_ov066_02118188();    /* pattern 5 run */
    int  func_ov066_021184c0();    /* pattern 5 enter */
    int  func_ov066_021184e0();    /* pattern 4 run */
    int  func_ov066_021185e4();    /* pattern 4 enter */
    int  func_ov066_02118604();    /* waiting run */
    int  func_ov066_02118658();    /* waiting enter */
    int  func_ov066_02118678();    /* rise run */
    int  func_ov066_021187c8();    /* rise enter */
    int  func_ov066_021188b0();    /* pattern-running run */
    int  func_ov066_02118934();    /* pattern-running enter */
    s32  func_ov066_02118954();    /* pattern 9 run (hand) */
    int  func_ov066_021189a0();    /* pattern 9 enter (hand) */
    int  func_ov066_021189c0();    /* pattern 8 run (hand) */
    int  func_ov066_02118a30();    /* pattern 8 enter (hand) */
    s32  func_ov066_02118a50();    /* pattern 7 run (hand) */
    int  func_ov066_02118b08();    /* pattern 7 enter (hand) */
    s32  func_ov066_02118b28();    /* pattern 6 run (hand) */
    int  func_ov066_02118be0();    /* pattern 6 enter (hand) */
    s32  func_ov066_02118c00();    /* pattern 5 run (hand) */
    int  func_ov066_02118cb8();    /* pattern 5 enter (hand) */
    int  func_ov066_02118cdc();    /* pattern 4 run (hand) */
    int  func_ov066_02118de0();    /* pattern 4 enter (hand) */
    int  func_ov066_02118e04();    /* decision run */
    int  func_ov066_0211901c();    /* decision enter */
    int  func_ov066_0211903c();    /* talk run */
    int  func_ov066_02119348();    /* talk enter */
    int  func_ov066_02119398();    /* dormant run */
    int  func_ov066_0211944c();    /* dormant enter (does nothing) */
    int  func_ov066_02119454(void *pv); /* install a state descriptor */
    void func_ov066_021194a4();    /* refresh the collision matrix */
    void func_ov066_021194fc();    /* refresh the model matrices */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char Eyerok_size_must_be_0x874[sizeof(Eyerok) == 0x874 ? 1 : -1];
#endif

#else

#include "Model.h"

/* The C spelling of the same object, flat. Retained for any leftover C
   translation unit, same arrangement as include/dExtShadowModel_c.h. */
struct Eyerok {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x26];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x44];
    /* Model member. The cartridge's own ~Eyerok calls _ZN5ModelD1Ev at +0x0d4 (D0/D1),
       a relocation the ROM build checks; recovered by tools/dtor_members.py. D1 and not
       D2, so it is this type and not an inlined base. */
    Model mModel1;            /* 0x0d4 */
    /* dBgW_KcMbg member. The cartridge's own ~Eyerok calls _ZN10dBgW_KcMbgD1Ev at
       +0x124 (D0/D1), a relocation the ROM build checks; recovered by
       tools/dtor_members.py. D1 and not D2, so it is this type and not an inlined base. */
    dBgW_KcMbg mMeshCollider;            /* 0x124 */
    u8  pad_2ec[0x34];
    u8  mdCcAcPos_c;            /* 0x320 */
    u8  pad_321[0x33];
    s32 mdCcAcPos_c_posX;            /* 0x354 */
    s32 mdCcAcPos_c_posY;            /* 0x358 */
    s32 mdCcAcPos_c_posZ;            /* 0x35c */
    u8  mBlendModelAnim;            /* 0x360 */
    u8  pad_361[0x6f];
    Model mModel2;            /* 0x3d0 */
    u8  mShadowModel;            /* 0x420 */
    u8  pad_421[0x27];
    u8  mTextureSequence;            /* 0x448 */
    u8  pad_449[0x13];
    s32 mShadowMtx[12];              /* 0x45c */
    void *mState;            /* 0x48c */
    void *mTalkPlayer;       /* 0x490 */
    s32 mStateWork0;         /* 0x494 */
    s32 mStateWork1;         /* 0x498 */
    s32 mPartIdx;            /* 0x49c */
    s32 mSubState;           /* 0x4a0 */
    s32 mRestPosX;            /* 0x4a4 */
    s32 mRestPosY;            /* 0x4a8 */
    s32 mRestPosZ;            /* 0x4ac */
    s32 mSpawnPosX;            /* 0x4b0 */
    s32 mSpawnPosY;            /* 0x4b4 */
    s32 mSpawnPosZ;            /* 0x4b8 */
    s32 mTargetPosX;        /* 0x4bc */
    s32 mTargetPosY;        /* 0x4c0 */
    s32 mTargetPosZ;        /* 0x4c4 */
    s32 mHorzAccelStep;     /* 0x4c8 */
    u8  pad_4cc[0x4];
    u16 mTimer1;            /* 0x4d0 */
    u16 mTimer2;            /* 0x4d2 */
    u16 mDustCounter;            /* 0x4d4 */
    u8  pad_4d6[0x2];
    s8  mHitPoints;            /* 0x4d8 */
    u8  mPickCount;         /* 0x4d9 */
    u8  pad_4da[0x2];
    struct Vector3 mDustPos[0x14];    /* 0x4dc */
    u32 mDustParticle1[0x14];        /* 0x5cc */
    u32 mDustParticle2[0x14];        /* 0x61c */
    u16 mTexTimer;          /* 0x66c */
    u8  mTexPhase;          /* 0x66e */
    u8  pad_66f[0x3];
    u8  mStarId;            /* 0x672 */
    u8  mStarTracked;            /* 0x673 */
    /* dBgW_KcMbg member. The cartridge's own ~Eyerok calls _ZN10dBgW_KcMbgD1Ev at
       +0x674 (D0/D1), a relocation the ROM build checks; recovered by
       tools/dtor_members.py. D1 and not D2, so it is this type and not an inlined base. */
    dBgW_KcMbg mMeshCollider2;            /* 0x674 */
    struct Matrix4x3 mClsnMat2;    /* 0x83c */
    s32 mHandUniqueID1;            /* 0x86c */
    s32 mHandUniqueID2;            /* 0x870 */
};

#endif /* __cplusplus */

#endif
