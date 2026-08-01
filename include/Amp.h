/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Amp: 5 matched functions, 13 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef AMP_H
#define AMP_H
#include "types.h"

struct Amp {
    u8  pad_000[0x8];
    u32 unk_008;            /* 0x008 */
    u8  pad_00c[0x74];
    u8  unk_080;            /* 0x080 */
    u8  pad_081[0x1b];
    s32 unk_09c;            /* 0x09c */
    s32 unk_0a0;            /* 0x0a0 */
    u8  pad_0a4[0x30];
    u8  mModelAnim;            /* 0x0d4 */
    u8  pad_0d5[0x63];
    u8  mModel;            /* 0x138 */
    u8  pad_139[0x4f];
    u8  mTextureSequence;            /* 0x188 */
    u8  pad_189[0x13];
    u8  mTextureTransformer;            /* 0x19c */
    u8  pad_19d[0x13];
    u8  mShadowModel;            /* 0x1b0 */
    u8  pad_1b1[0x27];
    u8  mMovingCylinderClsnWithPos;            /* 0x1d8 */
    u8  pad_1d9[0x3f];
    u8  mWithMeshClsn;            /* 0x218 */
    u8  pad_219[0x1bb];
    u8  unk_3d4;            /* 0x3d4 */
    u8  pad_3d5[0x4b];
    s32 unk_420;            /* 0x420 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
    int Render();
#endif
};

#endif
