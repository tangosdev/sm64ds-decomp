/* class Eyerok : dBgActor_c. Real C++ form.
 *
 * Base and size from the factory (src/game/actors/Eyerok/Eyerok_Spawn.cpp): fBase_c::operator
 * new(0x874), then dBgActor_c::dBgActor_c(), then stores _ZTV6Eyerok. Two
 * vtable stores in the destructor -- own, then dBgActor_c's -- confirming a
 * DIRECT dBgActor_c child, no intermediate. The apparent third store some
 * tooling flagged is `extern int _ZN7Vector3D1Ev[];` passed as a callback
 * pointer to `__destroy_arr` (see the D1 body) -- a literal-pool FUNCTION
 * POINTER argument, not a vptr store; it never writes to `this`.
 *
 * dBgActor_c ends at 0x320. Every member below closes exactly on the next
 * (confirmed against src/game/actors/Eyerok/Eyerok_Spawn.cpp and src/_ZN6EyerokD1Ev.c, which
 * construct/destroy each in this order):
 *
 *     dCcAcPos_c  0x320 + 0x40 = 0x360
 *     BlendModelAnim             0x360 + 0x70 = 0x3d0
 *     Model                      0x3d0 + 0x50 = 0x420
 *     ShadowModel                0x420 + 0x28 = 0x448
 *     TextureSequence            0x448 + 0x14 = 0x45c
 *
 * 0x45c..0x4d6 is a run of individually evidenced scalars. At 0x4dc,
 * Vector3[0x14] (0xc == sizeof(Vector3)) -- destroyed with
 * __destroy_arr(ptr, 0x14, 0xc, _ZN7Vector3D1Ev), same evidence shape as
 * include/Unagi.h's mStarUniqueID -- ends at 0x5cc.
 *
 * THE CLASS NOW CLOSES ON ITS OWN SIZE. Reading Behavior and InitResources as
 * named members (see notes/bgobject-provenance.md) turned every remaining pad
 * in this class into an evidenced field: the 0xa8 bytes after the Vector3
 * array are two 0x14-entry particle-handle arrays plus the star id/tracked
 * pair the ROM writes at 0x672/0x673, and the "unused tail" at 0x83c is the
 * Matrix4x3 InitResources passes to dBgW_KcMbg::SetFile followed by the two
 * uniqueIDs of the hands it spawns. 0x870 + 4 = 0x874, which is exactly the
 * literal src/game/actors/Eyerok/Eyerok_Spawn.cpp passes to operator new -- the size is now
 * corroborated by the field span rather than merely asserted over it.
 *
 * 0x674 is a second, class-owned dBgW_KcMbg (named by _ZN10dBgW_KcMbgD1Ev in
 * the destructor), distinct from dBgActor_c's own at 0x124.
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
#include "ShadowModel.h"
#include "TextureSequence.h"

struct Eyerok : dBgActor_c {
    dCcAcPos_c mdCcAcPos_c;  /* 0x320 */
    BlendModelAnim mBlendModelAnim;                        /* 0x360 */
    Model mModel2;                                         /* 0x3d0 */
    ShadowModel mShadowModel;                               /* 0x420 */
    TextureSequence mTextureSequence;                       /* 0x448 */
    u8  unk_45c[0x30];
    /* Behavior loads this word and calls through the pointer-to-member at
       +8 of what it points at, and compares it against &data_ov066_0211b07c --
       so it is a pointer to the current state descriptor, not a byte.
       func_ov066_02119454 is what installs one. */
    void *mState;                                           /* 0x48c */
    u8  pad_490[0xc];
    s32 mPartIdx;                                           /* 0x49c */
    u8  pad_4a0[0x4];
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
    u8  pad_4bc[0x14];
    /* Both are counted down once a frame by DecIfAbove0_Short, which takes a
       u16 * -- 0x4d0 was declared u8 + 1 byte of padding until that was read. */
    u16 mTimer1;                                            /* 0x4d0 */
    u16 mTimer2;                                            /* 0x4d2 */
    u16 mDustCounter;                                       /* 0x4d4 */
    u8  pad_4d6[0x2];
    s8  unk_4d8;                                            /* 0x4d8 */
    u8  pad_4d9[0x3];
    /* The ROM destroys this with __destroy_arr(this + 0x4dc, 0x14, 0xc,
       _ZN7Vector3D1Ev) -- 0x14 elements, 0xc == sizeof(Vector3), same
       evidence shape as include/Unagi.h's mStarUniqueID. Only raw
       `this + 0x4dc` / `+ 0x4e0` / `+ 0x4e4` offsets are read elsewhere
       (one Vector3), so the count is trusted from the destructor call, not
       from any indexed access. */
    Vector3 mDustPos[0x14];                                 /* 0x4dc */
    /* One recycled Particle::System handle per mDustPos slot, per effect id.
       Behavior walks all 0x14 slots and reissues both. Was pad_5cc. */
    u32 mDustParticle1[0x14];                               /* 0x5cc */
    u32 mDustParticle2[0x14];                               /* 0x61c */
    u8  pad_66c[0x6];
    u8  mStarId;                                            /* 0x672 */
    u8  mStarTracked;                                       /* 0x673 */
    dBgW_KcMbg mMeshCollider2;                              /* 0x674 -- this class's own, not dBgActor_c's */
    /* NOT unused tail. InitResources passes `this + 0x83c` as the Matrix4x3 &
       argument of dBgW_KcMbg::SetFile on every path, and writes the two words
       after it with the uniqueIDs of the two hands it spawns. 0x83c + 0x30 =
       0x86c, and 0x870 + 4 = 0x874, the literal src/game/actors/Eyerok/Eyerok_Spawn.cpp passes to
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
};

typedef char Eyerok_size_must_be_0x874[sizeof(Eyerok) == 0x874 ? 1 : -1];

#else

#include "Model.h"

/* The C spelling of the same object, flat. Retained for any leftover C
   translation unit, same arrangement as include/ShadowModel.h. */
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
    u8  pad_449[0x43];
    void *mState;            /* 0x48c */
    u8  pad_490[0xc];
    s32 mPartIdx;            /* 0x49c */
    u8  pad_4a0[0x4];
    s32 mRestPosX;            /* 0x4a4 */
    s32 mRestPosY;            /* 0x4a8 */
    s32 mRestPosZ;            /* 0x4ac */
    s32 mSpawnPosX;            /* 0x4b0 */
    s32 mSpawnPosY;            /* 0x4b4 */
    s32 mSpawnPosZ;            /* 0x4b8 */
    u8  pad_4bc[0x14];
    u16 mTimer1;            /* 0x4d0 */
    u16 mTimer2;            /* 0x4d2 */
    u16 mDustCounter;            /* 0x4d4 */
    u8  pad_4d6[0x2];
    s8  unk_4d8;            /* 0x4d8 */
    u8  pad_4d9[0x3];
    struct Vector3 mDustPos[0x14];    /* 0x4dc */
    u32 mDustParticle1[0x14];        /* 0x5cc */
    u32 mDustParticle2[0x14];        /* 0x61c */
    u8  pad_66c[0x6];
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
