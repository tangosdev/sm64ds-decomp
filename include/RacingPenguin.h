/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class RacingPenguin: 5 matched functions, 7 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef RACINGPENGUIN_H
#define RACINGPENGUIN_H
#include "types.h"
#include "ModelAnim.h"
#include "TextureSequence.h"
#include "ShadowModel.h"
#include "dCcAc_c.h"
#include "dBgCh_Actr.h"
#include "PathPtr.h"

struct RacingPenguin {
    u8  pad_000[0x80];
    /* 0x080..0x0d4 is dActor_c's, and dActor_c.h is de-bannered -- hand-reconstructed, not generated. Was one u8
       marker over the whole range. */
    s32 mScaleX;                 /* 0x080 */
    s32 mScaleY;                 /* 0x084 */
    s32 mScaleZ;                 /* 0x088 */
    s16 mAngleX;                 /* 0x08c */
    s16 mAngleY;                 /* 0x08e */
    s16 mAngleZ;                 /* 0x090 */
    s16 mPrevAngleX;             /* 0x092 */
    s16 mPrevAngleY;             /* 0x094 */
    s16 mPrevAngleZ;             /* 0x096 */
    s32 mHorzSpeed;              /* 0x098 */
    s32 mVertAccel;              /* 0x09c */
    s32 mTerminalVelocity;       /* 0x0a0 */
    u8  pad_0a4[0x4];
    s32 mVertSpeed;              /* 0x0a8 */
    u8  pad_0ac[0x4];
    u32 mFlags;                  /* 0x0b0 */
    s32 mClipOffsetY;                 /* 0x0b4 */
    s32 mClipRadius;                 /* 0x0b8 */
    s32 mClipDistance;                 /* 0x0bc */
    s32 mFarDistance;                 /* 0x0c0 */
    u8  mClipResult;                 /* 0x0c4 */
    u8  pad_0c5[0x7];
    s8  mAreaId;                 /* 0x0cc */
    u8  pad_0cd[0x1];
    s16 mDeathTableID;                 /* 0x0ce */
    u8  pad_0d0[0x4];
    /* ModelAnim member, named by _ZN9ModelAnimD1Ev at +0xd4 -- a relocation the ROM build
       checks. D1 and not D2, so it is this type and not an inlined base. The marker's pad
       stopped short of the object, so the member also takes over mAnimation (+0x50 = the
       Animation base), which the header declared separately inside it. */
    ModelAnim mModelAnim;            /* 0x0d4 */
    /* TextureSequence member, named by the class's own destructor calling
       TextureSequence's D1 at +0x138 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN13RacingPenguinD0Ev.c] */
    TextureSequence mTextureSequence;            /* 0x138 */
    /* ShadowModel member, named by the class's own destructor calling
       ShadowModel's D1 at +0x14c -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN13RacingPenguinD0Ev.c] */
    ShadowModel mShadowModel;            /* 0x14c */
    /* dCcAc_c member, named by the class's own destructor calling
       dCcAc_c's D1 at +0x174 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN13RacingPenguinD0Ev.c] */
    dCcAc_c mdCcAc_c;            /* 0x174 */
    /* dBgCh_Actr member, named by the class's own destructor calling
       dBgCh_Actr's D1 at +0x1a8 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN13RacingPenguinD0Ev.c] */
    dBgCh_Actr mWithMeshClsn;            /* 0x1a8 */
    /* PathPtr: InitResources calls PathPtr::FromID on +0x364 and then
       PathPtr::GetNode on it -- both take the address, so this is the object
       and not a word inside one. */
    PathPtr mPath;            /* 0x364 */
    /* The node index GetNode is handed, by value, from +0x36c -- so it is a
       field of the penguin and not part of the two-word PathPtr above. */
    s32 mPathNodeIndex;            /* 0x36c */
    u8  pad_370[0x26];
    /* InitResources stores dActor_c::TrackStar's result here. It is the last
       byte of the object: 0x397 rounds to the 0x398 RacingPenguin_Spawn
       allocates. */
    u8  unk_396;            /* 0x396 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
    void OnPendingDestroy();                 /* slot 12 -- empty body in the ROM */
    int Render();
#endif
};

typedef char RacingPenguin_size_must_be_0x398[sizeof(struct RacingPenguin) == 0x398 ? 1 : -1];

#endif
