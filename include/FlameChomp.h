/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class FlameChomp: 5 matched functions, 13 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef FLAMECHOMP_H
#define FLAMECHOMP_H
#include "types.h"

struct FlameChomp {
    u8  pad_000[0x5c];
    /* Actor::mPosX -- Actor.h declares s32 here, and it is de-bannered (hand-reconstructed). */
    s32 unk_05c;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    u8  pad_064[0x1c];
    s32 mScaleX;            /* 0x080 */
    s32 mScaleY;            /* 0x084 */
    s32 mScaleZ;            /* 0x088 */
    u8  pad_08c[0x10];
    s32 unk_09c;            /* 0x09c */
    s32 unk_0a0;            /* 0x0a0 */
    u8  pad_0a4[0x30];
    u8  mModelAnim;            /* 0x0d4 */
    u8  pad_0d5[0x63];
    u8  mShadowModel;            /* 0x138 */
    u8  pad_139[0x27];
    u8  mMovingCylinderClsnWithPos;            /* 0x160 */
    u8  pad_161[0x3f];
    u8  mWithMeshClsn;            /* 0x1a0 */
    u8  pad_1a1[0x1bb];
    u8  unk_35c;            /* 0x35c */
    u8  pad_35d[0x4b];
    s32 unk_3a8;            /* 0x3a8 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
    int Render();
#endif
};

#endif
