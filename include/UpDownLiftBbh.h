/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class UpDownLiftBbh: 6 matched functions, 21 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef UPDOWNLIFTBBH_H
#define UPDOWNLIFTBBH_H
#include "types.h"

struct UpDownLiftBbh {
    u8  pad_000[0xc];
    /* 0x00c..0x060 is ActorBase's, and ActorBase.h is de-bannered -- hand-reconstructed, not generated. Was one u8
       marker over the whole range. */
    u16 unk_00c;                 /* 0x00c */
    u8  aliveState;              /* 0x00e */
    u8  shouldBeKilled;          /* 0x00f */
    u8  unk_010;                 /* 0x010 */
    u8  unk_011;                 /* 0x011 */
    u8  unk_012;                 /* 0x012 */
    u8  unk_013;                 /* 0x013 */
    u8  sceneNode[0x14];               /* 0x014 */
    u8  behavNode[0x10];               /* 0x028 */
    u8  renderNode[0x10];              /* 0x038 */
    u8  pad_048[0x18];
    /* 0x060..0x08e is Actor's, and Actor.h is de-bannered -- hand-reconstructed, not generated. Was one u8
       marker over the whole range. */
    s32 unk_060;                 /* 0x060 */
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
    s16 mAngleX;                 /* 0x08c */
    /* 0x08e..0x092 is Actor's, and Actor.h is de-bannered -- hand-reconstructed, not generated. Was one u8
       marker over the whole range. */
    s16 unk_08e;                 /* 0x08e */
    s16 mAngleZ;                 /* 0x090 */
    /* 0x092..0x096 is Actor's, and Actor.h is de-bannered -- hand-reconstructed, not generated. Was one u8
       marker over the whole range. */
    s16 unk_092;                 /* 0x092 */
    s16 mPrevAngleY;             /* 0x094 */
    /* 0x096..0x0d4 is Actor's, and Actor.h is de-bannered -- hand-reconstructed, not generated. Was one u8
       marker over the whole range. */
    s16 unk_096;                 /* 0x096 */
    s32 mHorzSpeed;              /* 0x098 */
    s32 mVertAccel;              /* 0x09c */
    s32 mTerminalVelocity;       /* 0x0a0 */
    u8  pad_0a4[0x4];
    s32 mVertSpeed;              /* 0x0a8 */
    u8  pad_0ac[0x4];
    u32 mFlags;                  /* 0x0b0 */
    s32 unk_0b4;                 /* 0x0b4 */
    s32 unk_0b8;                 /* 0x0b8 */
    s32 unk_0bc;                 /* 0x0bc */
    s32 unk_0c0;                 /* 0x0c0 */
    u8  unk_0c4;                 /* 0x0c4 */
    u8  pad_0c5[0x7];
    s8  mAreaId;                 /* 0x0cc */
    u8  pad_0cd[0x1];
    s16 unk_0ce;                 /* 0x0ce */
    u8  pad_0d0[0x4];
    u8  mModel;            /* 0x0d4 */
    u8  pad_0d5[0x4f];
    u8  mMeshCollider;            /* 0x124 */
    u8  pad_125[0x1db];
    u16 unk_300;            /* 0x300 */
    u8  pad_302[0x1e];
    s32 unk_320;            /* 0x320 */
    s32 unk_324;            /* 0x324 */
    s32 mVariant;            /* 0x328 */
    s32 mState;            /* 0x32c */
    s32 unk_330;            /* 0x330 */
    u8  unk_334;            /* 0x334 */
    u8  pad_335[0x3];
    u8  unk_338;            /* 0x338 */
    u8  pad_339[0x3];
    u8  unk_33c;            /* 0x33c */
    u8  pad_33d[0x3];
    u8  unk_340;            /* 0x340 */
    u8  pad_341[0x5];
    u8  unk_346;            /* 0x346 */
    u8  unk_347;            /* 0x347 */
    u8  unk_348;            /* 0x348 */
    u8  unk_349;            /* 0x349 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
#endif
};

#endif
