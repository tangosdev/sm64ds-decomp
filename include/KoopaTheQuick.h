/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class KoopaTheQuick: 6 matched functions, 38 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef KOOPATHEQUICK_H
#define KOOPATHEQUICK_H
#include "types.h"

struct KoopaTheQuick {
    u8  pad_000[0x8];
    /* 0x008..0x05c is ActorBase's, and ActorBase.h is de-bannered -- hand-reconstructed, not generated. Was one u8
       marker over the whole range. */
    u32 mParam;                  /* 0x008 */
    u16 actorID;                 /* 0x00c */
    u8  aliveState;              /* 0x00e */
    u8  shouldBeKilled;          /* 0x00f */
    u8  unk_010;                 /* 0x010 */
    u8  unk_011;                 /* 0x011 */
    u8  unk_012;                 /* 0x012 */
    u8  unk_013;                 /* 0x013 */
    u8  sceneNode[0x14];               /* 0x014 */
    u8  behavNode[0x10];               /* 0x028 */
    u8  renderNode[0x10];              /* 0x038 */
    u8  pad_048[0x14];
    /* Actor::mPosX -- Actor.h declares s32 here, and it is de-bannered (hand-reconstructed). */
    s32 unk_05c;            /* 0x05c */
    /* Actor::mPosY -- Actor.h declares s32 here, and it is de-bannered (hand-reconstructed). */
    s32 unk_060;            /* 0x060 */
    /* 0x064..0x080 is Actor's, and Actor.h is de-bannered -- hand-reconstructed, not generated. Was one u8
       marker over the whole range. */
    s32 unk_064;                 /* 0x064 */
    s32 unk_068;                 /* 0x068 */
    s32 unk_06c;                 /* 0x06c */
    s32 unk_070;                 /* 0x070 */
    s32 mCamSpacePosX;           /* 0x074 */
    s32 mCamSpacePosY;           /* 0x078 */
    s32 mCamSpacePosZ;           /* 0x07c */
    /* Actor::mScaleX -- Actor.h declares s32 here, and it is de-bannered (hand-reconstructed). */
    s32 mScaleX;            /* 0x080 */
    /* Actor::mScaleY -- Actor.h declares s32 here, and it is de-bannered (hand-reconstructed). */
    s32 mScaleY;            /* 0x084 */
    /* Actor::mScaleZ -- Actor.h declares s32 here, and it is de-bannered (hand-reconstructed). */
    s32 mScaleZ;            /* 0x088 */
    /* Actor::mAngleX -- Actor.h declares s16 here, and it is de-bannered (hand-reconstructed). */
    s16 unk_08c;            /* 0x08c */
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x4];
    s16 mPrevAngleY;            /* 0x094 */
    u8  pad_096[0x6];
    /* Actor::mVertAccel -- Actor.h declares s32 here, and it is de-bannered (hand-reconstructed). */
    s32 unk_09c;            /* 0x09c */
    /* 0x0a0..0x110 is Actor's, and Actor.h is de-bannered -- hand-reconstructed, not generated. Was one u8
       marker over the whole range. */
    s32 unk_0a0;                 /* 0x0a0 */
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
    u8  pad_0d0[0x40];
    u8  mMovingCylinderClsn;            /* 0x110 */
    u8  pad_111[0x33];
    u8  mWithMeshClsn;            /* 0x144 */
    u8  pad_145[0x1bb];
    u8  mModelAnim;            /* 0x300 */
    u8  pad_301[0x4f];
    u8  unk_350;            /* 0x350 */
    u8  pad_351[0x13];
    u8  mShadowModel;            /* 0x364 */
    u8  pad_365[0x27];
    s32 unk_38c;            /* 0x38c */
    u8  pad_390[0x4];
    s32 unk_394;            /* 0x394 */
    s32 unk_398;            /* 0x398 */
    s32 unk_39c;            /* 0x39c */
    s32 unk_3a0;            /* 0x3a0 */
    u8  unk_3a4;            /* 0x3a4 */
    u8  pad_3a5[0x5];
    s16 unk_3aa;            /* 0x3aa */
    u8  unk_3ac;            /* 0x3ac */
    u8  unk_3ad;            /* 0x3ad */
    u8  unk_3ae;            /* 0x3ae */
    u8  pad_3af[0x1];
    u8  unk_3b0;            /* 0x3b0 */
    u8  unk_3b1;            /* 0x3b1 */
    u8  pad_3b2[0x2];
    u8  unk_3b4;            /* 0x3b4 */
    u8  unk_3b5;            /* 0x3b5 */
    u8  unk_3b6;            /* 0x3b6 */
    u8  pad_3b7[0x1];
    s32 unk_3b8;            /* 0x3b8 */
    s32 unk_3bc;            /* 0x3bc */
    s32 unk_3c0;            /* 0x3c0 */
    s32 unk_3c4;            /* 0x3c4 */
    u8  unk_3c8;            /* 0x3c8 */
    u8  pad_3c9[0xf];
    u8  mPathPtr;            /* 0x3d8 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
#endif
};

#endif
