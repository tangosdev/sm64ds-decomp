/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class HootTheOwl: 5 matched functions, 24 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef HOOTTHEOWL_H
#define HOOTTHEOWL_H
#include "types.h"

struct HootTheOwl {
    u8  pad_000[0x8c];
    s16 unk_08c;            /* 0x08c */
    s16 unk_08e;            /* 0x08e */
    s16 unk_090;            /* 0x090 */
    s16 unk_092;            /* 0x092 */
    s16 unk_094;            /* 0x094 */
    s16 unk_096;            /* 0x096 */
    u8  pad_098[0x4];
    s32 unk_09c;            /* 0x09c */
    s32 unk_0a0;            /* 0x0a0 */
    u8  pad_0a4[0x4];
    s32 unk_0a8;            /* 0x0a8 */
    s32 unk_0ac;            /* 0x0ac */
    u8  pad_0b0[0x50];
    u8  unk_100;            /* 0x100 */
    u8  pad_101[0xf];
    u8  mMovingCylinderClsnWithPos;            /* 0x110 */
    u8  pad_111[0x17];
    u8  unk_128;            /* 0x128 */
    u8  pad_129[0x27];
    u8  mWithMeshClsn;            /* 0x150 */
    u8  pad_151[0x1bb];
    u8  mModelAnim;            /* 0x30c */
    u8  pad_30d[0x4f];
    u8  mAnimation;            /* 0x35c */
    u8  pad_35d[0x7];
    s32 unk_364;            /* 0x364 */
    s32 unk_368;            /* 0x368 */
    u8  pad_36c[0x4];
    u8  mShadowModel;            /* 0x370 */
    u8  pad_371[0x57];
    u8  mCurrentState;            /* 0x3c8 */
    u8  pad_3c9[0x3];
    s32 unk_3cc;            /* 0x3cc */
    u8  pad_3d0[0x4];
    u8  unk_3d4;            /* 0x3d4 */
    u8  pad_3d5[0xf];
    u8  unk_3e4;            /* 0x3e4 */
    u8  pad_3e5[0xb];
    s32 unk_3f0;            /* 0x3f0 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
    int Render();
#endif
};

#endif
