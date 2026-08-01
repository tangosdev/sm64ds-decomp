/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class BulletBill: 5 matched functions, 20 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef BULLETBILL_H
#define BULLETBILL_H
#include "types.h"

struct BulletBill {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0xc];
    u8  unk_074;            /* 0x074 */
    u8  pad_075[0x17];
    s16 mAngleX;            /* 0x08c */
    s16 mAngleY;            /* 0x08e */
    s16 mAngleZ;            /* 0x090 */
    s16 mPrevAngleX;            /* 0x092 */
    s16 mPrevAngleY;            /* 0x094 */
    s16 mPrevAngleZ;            /* 0x096 */
    u8  pad_098[0x68];
    s16 unk_100;            /* 0x100 */
    u8  pad_102[0xe];
    u8  mMovingCylinderClsnWithPos;            /* 0x110 */
    u8  pad_111[0x1f];
    s32 unk_130;            /* 0x130 */
    u32 unk_134;            /* 0x134 */
    u8  pad_138[0x18];
    u8  mWithMeshClsn;            /* 0x150 */
    u8  pad_151[0x1bb];
    u8  mModel1;            /* 0x30c */
    u8  pad_30d[0x4f];
    u8  mModel2;            /* 0x35c */
    u8  pad_35d[0x4f];
    u8  mShadowModel;            /* 0x3ac */
    u8  pad_3ad[0x27];
    s32 mState;            /* 0x3d4 */
    s32 unk_3d8;            /* 0x3d8 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
    int Render();
#endif
};

#endif
