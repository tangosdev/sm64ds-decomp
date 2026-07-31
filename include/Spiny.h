/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Spiny: 5 matched functions, 15 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef SPINY_H
#define SPINY_H
#include "types.h"

struct Spiny {
    u8  pad_000[0x60];
    s32 mPosY;            /* 0x060 */
    u8  pad_064[0x1c];
    s32 mScaleX;            /* 0x080 */
    s32 mScaleY;            /* 0x084 */
    s32 mScaleZ;            /* 0x088 */
    u8  unk_08c;            /* 0x08c */
    u8  pad_08d[0x5];
    u8  unk_092;            /* 0x092 */
    u8  pad_093[0x1d];
    u8  unk_0b0;            /* 0x0b0 */
    u8  pad_0b1[0x23];
    u8  mModel;            /* 0x0d4 */
    u8  pad_0d5[0x4f];
    u8  mModelAnim;            /* 0x124 */
    u8  pad_125[0x63];
    u8  mShadowModel;            /* 0x188 */
    u8  pad_189[0x27];
    u8  mMovingCylinderClsn;            /* 0x1b0 */
    u8  pad_1b1[0x33];
    u8  mWithMeshClsn;            /* 0x1e4 */
    u8  pad_1e5[0x1bb];
    u8  unk_3a0;            /* 0x3a0 */
    u8  pad_3a1[0x37];
    s32 unk_3d8;            /* 0x3d8 */
    u8  pad_3dc[0xd];
    u8  unk_3e9;            /* 0x3e9 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
    int Render();
#endif
};

#endif
