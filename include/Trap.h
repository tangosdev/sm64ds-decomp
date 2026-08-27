#ifndef TRAP_H
#define TRAP_H

#include "types.h"

/* Derives from dBgActor_c: the destructor stores this class's vtable, then
 * dBgActor_c's -- inlined -- then destroys the dBgW_KcMbg at 0x124 and
 * the Model at 0xd4 before chaining to dActor_c. All three belong to dBgActor_c.
 * Everything this header used to restate below 0x320 -- mPosX/mPosY/mPosZ at
 * 0x05c/0x060/0x064, mAngleY at 0x08e, mAreaId at 0x0cc -- was dActor_c's, and
 * is inherited now. See include/dActor_c.h.
 *
 * SIZE IS 0x3b0 (944 decimal), THE LITERAL Trap_Spawn.c passes to
 * fBase_c::operator new -- not merely the observed field span, though here
 * they agree: dBgActor_c ends at 0x320 and this class adds exactly one bare
 * Model (0x50, unlike the dBgCh_Actr siblings) plus trailing scalars,
 * landing on 0x3b0.
 *
 * THIS IS THE MID-RENAME CLASS. Before this change the header was a flat,
 * auto-generated `struct daObjC1_Trap_c` while the vtable and both
 * destructors already carried the real ROM name "Trap" -- _ZTV4Trap,
 * _ZN4TrapD1Ev, _ZN4TrapD0Ev. Only the RTTI pair, _ZTI14daObjC1_Trap_c and
 * _ZTS14daObjC1_Trap_c, still spelled the old placeholder name; this class
 * becoming its own key-function TU is what makes the compiler emit them
 * consistently, so config/arm9/overlays/ov010/symbols.txt renames those two
 * symbols in the same commit as this header (see
 * notes/actor-class-names-off-by-one.md and the vague-linkage rule in
 * decomp-cpp-class-form for why the rename cannot be split from the rewrite).
 */

#ifdef __cplusplus

#include "dBgActor_c.h"
#include "Model.h"

struct dActor_c;
struct Player;

struct Trap : dBgActor_c {
    /* Bare Model, not dBgCh_Actr -- Trap_Spawn.c calls
       _ZN5ModelC1Ev((char*)p + 0x320) directly, no dBgCh_Actr wrapper. This is
       the door's own model and it is NOT the only one the object carries:
       dBgActor_c's inherited Model at 0xd4 is live too -- ~Trap destroys both,
       Model::D1 at +0x320 and again at +0xd4 (see src/game/actors/Trap/_ZN4TrapD1Ev.cpp). So it
       must not be called "mModel": that name is already the base's, and an
       unqualified mModel inside a Trap method would silently bind to 0x320
       while a reader collapsing a +0xd4 poke would expect 0xd4. Named for the
       role InitResources and Render give it, beside mDoorMat below. */
    Model mDoorModel;                 /* 0x320 */
    /* A Matrix4x3, and the fit is exact from both sides: mDoorModel above ends at
       0x370 and mState below starts at 0x3a0, which is 0x30 -- sizeof(Matrix4x3)
       -- and InitResources hands this address to dBgW_KcMbg::SetFile, whose
       second parameter is `const Matrix4x3 &'. dBgActor_c's own mClsnMat at
       0x2ec is inherited and separate; this is the trap doors' own. */
    Matrix4x3 mDoorMat;               /* 0x370 */
    s32 mState;                      /* 0x3a0 */
    s32 mPlayerDist;                      /* 0x3a4 */
    u16 mOpenSpeed;                      /* 0x3a8 */
    u8  mTrapActive;                      /* 0x3aa */
    u8  mIsSpawner;                      /* 0x3ab */
    s32 mSpawnerID;                      /* 0x3ac */

    /* --- vtable --- */
    virtual ~Trap();

    int InitResources();
    int CleanupResources();
    int Behavior();
    int Render();
};

typedef char Trap_size_must_be_0x3b0[sizeof(Trap) == 0x3b0 ? 1 : -1];

#else

/* The C spelling of the same object, flat. Kept because the four vtable-slot
   override files still read the object by hand offset. The arrangement
   matches include/PushBlock.h and the other members of this family
   (DonutBlock.h, BigBrickBlock.h, MetalNet.h, PyramidStep.h). */
struct Trap {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x26];
    u16 mAngleY;            /* 0x08e */
    u8  pad_090[0x3c];
    s8  mAreaId;            /* 0x0cc */
    u8  pad_0cd[0x2d3];
    s32 mState;            /* 0x3a0 */
    s32 mPlayerDist;            /* 0x3a4 */
    u16 mOpenSpeed;            /* 0x3a8 */
    u8  mTrapActive;            /* 0x3aa */
    u8  mIsSpawner;            /* 0x3ab */
    s32 mSpawnerID;            /* 0x3ac */
};

#endif /* __cplusplus */

#endif /* TRAP_H */
