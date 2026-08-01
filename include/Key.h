/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Key: 6 matched functions, 36 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef KEY_H
#define KEY_H
#include "types.h"

struct Key {
    u8  pad_000[0x8];
    s32 mParam;            /* 0x008 */
    u16 mActorID;            /* 0x00c */
    u8  pad_00e[0x4e];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x18];
    s32 mScaleX;            /* 0x080 */
    s32 mScaleY;            /* 0x084 */
    s32 mScaleZ;            /* 0x088 */
    u8  pad_08c[0x2];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0xc];
    s32 unk_09c;            /* 0x09c */
    s32 unk_0a0;            /* 0x0a0 */
    u8  pad_0a4[0x4];
    s32 unk_0a8;            /* 0x0a8 */
    u8  pad_0ac[0x4];
    u32 unk_0b0;            /* 0x0b0 */
    u8  pad_0b4[0x1c];
    s32 mEatingPlayer;            /* 0x0d0 */
    u8  pad_0d4[0x2c];
    u16 unk_100;            /* 0x100 */
    u8  pad_102[0xe];
    s32 unk_110;            /* 0x110 */
    u8  mModelAnim;            /* 0x114 */
    u8  pad_115[0xf];
    u8  unk_124;            /* 0x124 */
    u8  pad_125[0x3];
    u8  unk_128;            /* 0x128 */
    u8  pad_129[0x3b];
    u8  mAnimation;            /* 0x164 */
    u8  pad_165[0xf];
    s32 unk_174;            /* 0x174 */
    u8  mModel;            /* 0x178 */
    u8  pad_179[0x4f];
    u8  mShadowModel;            /* 0x1c8 */
    u8  pad_1c9[0x57];
    u8  mMovingCylinderClsnWithPos;            /* 0x220 */
    u8  pad_221[0x3f];
    u8  mWithMeshClsn;            /* 0x260 */
    u8  pad_261[0x1bb];
    s32 unk_41c;            /* 0x41c */
    s32 unk_420;            /* 0x420 */
    s32 unk_424;            /* 0x424 */
    u8  pad_428[0x18];
    s16 mSpinSpeed;            /* 0x440 */
    u8  unk_442;            /* 0x442 */
    u8  unk_443;            /* 0x443 */
    s32 mState;            /* 0x444 */
    s32 unk_448;            /* 0x448 */
    u8  pad_44c[0x18];
    s32 unk_464;            /* 0x464 */
    s32 unk_468;            /* 0x468 */
    s32 unk_46c;            /* 0x46c */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
#endif
};

#endif
