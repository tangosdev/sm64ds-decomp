/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Moneybag: 5 matched functions, 19 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef MONEYBAG_H
#define MONEYBAG_H
#include "types.h"

struct Moneybag {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x18];
    s32 mScaleX;            /* 0x080 */
    s32 mScaleY;            /* 0x084 */
    s32 mScaleZ;            /* 0x088 */
    u8  pad_08c[0x10];
    s32 unk_09c;            /* 0x09c */
    s32 unk_0a0;            /* 0x0a0 */
    u8  pad_0a4[0xc];
    s32 unk_0b0;            /* 0x0b0 */
    u8  pad_0b4[0x20];
    u8  mModelAnim;            /* 0x0d4 */
    u8  pad_0d5[0x63];
    u8  mModel;            /* 0x138 */
    u8  pad_139[0x4f];
    u8  mShadowModel;            /* 0x188 */
    u8  pad_189[0x27];
    u8  mMovingCylinderClsn;            /* 0x1b0 */
    u8  pad_1b1[0x33];
    u8  mWithMeshClsn;            /* 0x1e4 */
    u8  pad_1e5[0x1bb];
    u8  unk_3a0;            /* 0x3a0 */
    u8  pad_3a1[0x2f];
    s32 unk_3d0;            /* 0x3d0 */
    s32 unk_3d4;            /* 0x3d4 */
    s32 unk_3d8;            /* 0x3d8 */
    u8  pad_3dc[0x14];
    u8  unk_3f0;            /* 0x3f0 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
    int Render();
#endif
};

#endif
