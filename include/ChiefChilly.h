/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class ChiefChilly: 5 matched functions, 21 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef CHIEFCHILLY_H
#define CHIEFCHILLY_H
#include "types.h"

struct ChiefChilly {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x18];
    s32 mScaleX;            /* 0x080 */
    s32 mScaleY;            /* 0x084 */
    s32 mScaleZ;            /* 0x088 */
    u8  pad_08c[0x2];
    s16 unk_08e;            /* 0x08e */
    u8  pad_090[0x4];
    s16 unk_094;            /* 0x094 */
    u8  pad_096[0x6];
    s32 unk_09c;            /* 0x09c */
    s32 unk_0a0;            /* 0x0a0 */
    u8  pad_0a4[0x6c];
    u8  mMovingCylinderClsnWithPos;            /* 0x110 */
    u8  pad_111[0x3f];
    u8  mWithMeshClsn;            /* 0x150 */
    u8  pad_151[0x1bb];
    u8  mBlendModelAnim;            /* 0x30c */
    u8  pad_30d[0x5b];
    s32 unk_368;            /* 0x368 */
    u8  pad_36c[0x14];
    u8  mShadowModel;            /* 0x380 */
    u8  pad_381[0x57];
    s32 unk_3d8;            /* 0x3d8 */
    s32 unk_3dc;            /* 0x3dc */
    s32 unk_3e0;            /* 0x3e0 */
    u8  pad_3e4[0xd8];
    s32 unk_4bc;            /* 0x4bc */
    u8  pad_4c0[0x5];
    u8  unk_4c5;            /* 0x4c5 */
    u8  pad_4c6[0x5];
    u8  unk_4cb;            /* 0x4cb */
#ifdef __cplusplus
    /* methods */
    int InitResources();
    int Render();
#endif
};

#endif
