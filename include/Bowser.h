/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Bowser: 5 matched functions, 44 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef BOWSER_H
#define BOWSER_H
#include "types.h"

struct Bowser {
    u8  pad_000[0x4];
    s32 unk_004;            /* 0x004 */
    s32 mParam;            /* 0x008 */
    u8  pad_00c[0x50];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x18];
    s32 mScaleX;            /* 0x080 */
    s32 mScaleY;            /* 0x084 */
    s32 mScaleZ;            /* 0x088 */
    u8  pad_08c[0x2];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x4];
    s16 mPrevAngleY;            /* 0x094 */
    u8  pad_096[0x6];
    s32 unk_09c;            /* 0x09c */
    s32 unk_0a0;            /* 0x0a0 */
    u8  pad_0a4[0x30];
    u8  mModelAnim;            /* 0x0d4 */
    u8  pad_0d5[0x4f];
    u8  mAnimation;            /* 0x124 */
    u8  pad_125[0xb];
    s32 unk_130;            /* 0x130 */
    u8  pad_134[0x4];
    u8  mTextureSequence;            /* 0x138 */
    u8  pad_139[0x13];
    u8  mWithMeshClsn;            /* 0x14c */
    u8  pad_14d[0x1bb];
    u8  mShadowModel;            /* 0x308 */
    u8  pad_309[0x57];
    u8  mMovingCylinderClsnWithPos;            /* 0x360 */
    u8  pad_361[0x3f];
    s32 mTargetPlayer;            /* 0x3a0 */
    u8  pad_3a4[0x4];
    s32 unk_3a8;            /* 0x3a8 */
    u8  pad_3ac[0x4];
    s32 unk_3b0;            /* 0x3b0 */
    s32 unk_3b4;            /* 0x3b4 */
    s32 unk_3b8;            /* 0x3b8 */
    u8  pad_3bc[0x30];
    s32 unk_3ec;            /* 0x3ec */
    u8  pad_3f0[0x8];
    s32 unk_3f8;            /* 0x3f8 */
    s16 unk_3fc;            /* 0x3fc */
    u8  pad_3fe[0x8];
    s16 unk_406;            /* 0x406 */
    u8  pad_408[0x4];
    s32 unk_40c;            /* 0x40c */
    u8  pad_410[0x4];
    s8  unk_414;            /* 0x414 */
    u8  pad_415[0x1];
    s8  unk_416;            /* 0x416 */
    u8  pad_417[0x5];
    u8  unk_41c;            /* 0x41c */
    u8  pad_41d[0x6];
    s8  unk_423;            /* 0x423 */
    s8  unk_424;            /* 0x424 */
    u8  pad_425[0x1];
    s8  unk_426;            /* 0x426 */
    s8  unk_427;            /* 0x427 */
    u8  pad_428[0x1];
    s8  unk_429;            /* 0x429 */
    s8  unk_42a;            /* 0x42a */
    u8  unk_42b;            /* 0x42b */
    u8  pad_42c[0x18];
    s8  unk_444;            /* 0x444 */
    u8  pad_445[0x1];
    s8  unk_446;            /* 0x446 */
    u8  pad_447[0x1];
    s32 unk_448;            /* 0x448 */
    s32 unk_44c;            /* 0x44c */
    s32 unk_450;            /* 0x450 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int Render();
#endif
};

#endif
