/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class CapEnemy: 11 matched functions, 10 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef CAPENEMY_H
#define CAPENEMY_H
#include "types.h"
#include "Model.h"

/* fwd */
struct Vector3;
struct v;
struct v_;
struct CapEnemy {
    u8  pad_000[0x8];
    u32 mParam;            /* 0x008 */
    u16 mActorID;            /* 0x00c */
    u8  pad_00e[0x4e];
    /* 0x05c: NOT a scalar, and deliberately left as a marker. Both evidence passes
       see only its address taken, never a sized load, and Actor.h:65 puts
       mPosX/mPosY/mPosZ here -- so this stands over the position triple. One derived
       header declares s32 at this offset, which describes the X component alone;
       adopting that would trade an unknown for a narrower wrong answer. */
    /* 0x05c..0x08c is Actor's, and Actor.h is de-bannered -- hand-reconstructed, not generated. Was one u8
       marker over the whole range. */
    s32 unk_05c;                 /* 0x05c */
    s32 mPosY;                   /* 0x060 */
    s32 mPosZ;                   /* 0x064 */
    s32 unk_068;                 /* 0x068 */
    s32 unk_06c;                 /* 0x06c */
    s32 unk_070;                 /* 0x070 */
    s32 mCamSpacePosX;           /* 0x074 */
    s32 mCamSpacePosY;           /* 0x078 */
    s32 mCamSpacePosZ;           /* 0x07c */
    s32 mScaleX;                 /* 0x080 */
    s32 mScaleY;                 /* 0x084 */
    s32 mScaleZ;                 /* 0x088 */
    /* 0x08c: same shape -- address-only evidence, and Actor.h:89 puts
       mAngleX/mAngleY/mAngleZ here. Stands over the rotation triple. */
    /* 0x08c..0x0b0 is Actor's, and Actor.h is de-bannered -- hand-reconstructed, not generated. Was one u8
       marker over the whole range. */
    s16 unk_08c;                 /* 0x08c */
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
    u32 unk_0b0;            /* 0x0b0 */
    u8  pad_0b4[0x18];
    s8  mAreaId;            /* 0x0cc */
    u8  pad_0cd[0x3];
    s32 mEatingPlayer;            /* 0x0d0 */
    u8  pad_0d4[0x3f];
    u8  unk_113;            /* 0x113 */
    /* Model member, named by _ZN5ModelD1Ev at +0x114 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    Model mModel;            /* 0x114 */
    u8  unk_164;            /* 0x164 */
#ifdef __cplusplus
    /* methods */
    int DestroyIfCapNotNeeded();
    int GetCapEatenOffIt(const Vector3 & v_);
    struct Actor * ReleaseCap(const Vector3 & v_);
    struct Actor * RespawnIfHasCap();
    void RenderCapModel(const Vector3 * v);
    void Unk_02005d94();
#endif
};

#endif
