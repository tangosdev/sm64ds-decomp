/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class ToxBox: 6 matched functions, 25 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef TOXBOX_H
#define TOXBOX_H
#include "types.h"

struct ToxBox {
    u8  pad_000[0x8];
    s32 mParam;            /* 0x008 */
    u8  pad_00c[0x50];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x24];
    s16 mAngleX;            /* 0x08c */
    s16 mAngleY;            /* 0x08e */
    s16 mAngleZ;            /* 0x090 */
    u8  pad_092[0x42];
    u8  mModel;            /* 0x0d4 */
    u8  pad_0d5[0x1b];
    u8  unk_0f0;            /* 0x0f0 */
    u8  pad_0f1[0x33];
    u8  mMeshCollider;            /* 0x124 */
    u8  pad_125[0x1fb];
    s32 unk_320;            /* 0x320 */
    u8  mWithMeshClsn;            /* 0x324 */
    u8  pad_325[0x1c3];
    u8  mMovingCylinderClsnWithPos;            /* 0x4e8 */
    u8  pad_4e9[0x3f];
    u8  unk_528;            /* 0x528 */
    u8  pad_529[0x2f];
    s32 unk_558;            /* 0x558 */
    s32 unk_55c;            /* 0x55c */
    s32 unk_560;            /* 0x560 */
    u8  pad_564[0x4];
    s32 unk_568;            /* 0x568 */
    s32 unk_56c;            /* 0x56c */
    s32 unk_570;            /* 0x570 */
    u8  unk_574;            /* 0x574 */
    u8  unk_575;            /* 0x575 */
    u8  pad_576[0x2];
    s32 unk_578;            /* 0x578 */
    s32 unk_57c;            /* 0x57c */
    u8  pad_580[0xc];
    u8  mPathPtr;            /* 0x58c */
#ifdef __cplusplus
    /* methods */
    int CleanupResources();
    int Render();
#endif
};

#endif
