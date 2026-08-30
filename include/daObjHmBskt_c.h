#ifndef DAOBJHMBSKT_C_H
#define DAOBJHMBSKT_C_H

#include "types.h"

/* Derives from dBgActor_c: the destructor stores this class's vtable, then
 * dBgActor_c's -- inlined -- then destroys the dBgW_KcMbg at 0x124 and
 * the Model at 0xd4 before chaining to dActor_c. All three belong to dBgActor_c.
 * Everything this header used to restate below 0x320 was dActor_c's and
 * dBgActor_c's, and is inherited now.
 *
 * SIZE IS 0x4e0, THE LITERAL UkikiCage_Spawn.c passes to fBase_c::operator
 * new (1248 decimal) -- not merely the observed field span, though here they
 * agree: dBgActor_c ends at 0x320 and this class adds exactly one dBgCh_Actr
 * (0x1bc) plus one trailing scalar, landing on 0x4e0 with no padding.
 *
 * THIS IS THE MID-RENAME CLASS. Before this change the header was a flat,
 * auto-generated `struct daObjHmBskt_c` while the vtable and both
 * destructors already carried the real ROM name "daObjHmBskt_c" --
 * _ZTV13daObjHmBskt_c, _ZN13daObjHmBskt_cD1Ev, _ZN13daObjHmBskt_cD0Ev. Only the RTTI pair,
 * _ZTI13daObjHmBskt_c and _ZTS13daObjHmBskt_c, still spelled the old
 * placeholder name; this class becoming its own key-function TU is what makes
 * the compiler emit them consistently, so config/arm9/overlays/ov030/symbols.txt
 * renames those two symbols in the same commit as this header (see
 * notes/actor-class-names-off-by-one.md and the vague-linkage rule in
 * decomp-cpp-class-form for why the rename cannot be split from the rewrite).
 */

#ifdef __cplusplus

#include "dBgActor_c.h"
#include "dBgCh_Actr.h"

struct dActor_c;

struct daObjHmBskt_c : dBgActor_c {
    dBgCh_Actr mWithMeshClsn;       /* 0x320 */
    dActor_c *mStarActor;            /* 0x4dc -- Spawn(0xb2, ...) result */

    /* --- vtable --- */
    virtual ~daObjHmBskt_c();

    int InitResources();
    int CleanupResources();
    int Behavior();
    int Render();
};

typedef char UkikiCage_size_must_be_0x4e0[sizeof(daObjHmBskt_c) == 0x4e0 ? 1 : -1];

#else

/* The C spelling of the same object, flat. Kept because the D0 file used to be
   compiler-generated C; the arrangement matches include/daObjPushblock_c.h and the
   other members of this family (DonutBlock.h, BigBrickBlock.h, MetalNet.h,
   daObjC1_Trap_c.h). */
struct daObjHmBskt_c {
    u8  pad_000[0x5c];
    s32 mPosX;              /* 0x05c */
    s32 mPosY;              /* 0x060 */
    s32 mPosZ;              /* 0x064 */
    u8  pad_068[0x18];
    s32 mScaleX;            /* 0x080 */
    s32 mScaleY;            /* 0x084 */
    s32 mScaleZ;            /* 0x088 */
    u8  pad_08c[0x2];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x8];
    s32 mHorzSpeed;         /* 0x098 */
    s32 mVertAccel;         /* 0x09c */
    s32 mTerminalVelocity;  /* 0x0a0 */
    u8  pad_0a4[0x28];
    s8  mAreaId;            /* 0x0cc */
    u8  pad_0cd[0x40f];
    s32 mStarActor;         /* 0x4dc */
};

#endif /* __cplusplus */

#endif /* DAOBJHMBSKT_C_H */
