/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class RacingPenguin: 5 matched functions, 7 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef RACINGPENGUIN_H
#define RACINGPENGUIN_H
#include "types.h"

struct RacingPenguin {
    u8  pad_000[0x80];
    u8  unk_080;            /* 0x080 */
    u8  pad_081[0x53];
    u8  mModelAnim;            /* 0x0d4 */
    u8  pad_0d5[0x4f];
    u8  mAnimation;            /* 0x124 */
    u8  pad_125[0x13];
    u8  mTextureSequence;            /* 0x138 */
    u8  pad_139[0x13];
    u8  mShadowModel;            /* 0x14c */
    u8  pad_14d[0x27];
    u8  mMovingCylinderClsn;            /* 0x174 */
    u8  pad_175[0x33];
    u8  mWithMeshClsn;            /* 0x1a8 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
    int Render();
#endif
};

#endif
