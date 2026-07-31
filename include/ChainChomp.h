/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class ChainChomp: 5 matched functions, 10 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef CHAINCHOMP_H
#define CHAINCHOMP_H
#include "types.h"

struct ChainChomp {
    u8  pad_000[0x60];
    s32 mPosY;            /* 0x060 */
    u8  pad_064[0x1c];
    u8  mScaleX;            /* 0x080 */
    u8  pad_081[0x8f];
    u8  mMovingCylinderClsnWithPos;            /* 0x110 */
    u8  pad_111[0x3f];
    u8  mModelAnim;            /* 0x150 */
    u8  pad_151[0x63];
    u8  mShadowModel;            /* 0x1b4 */
    u8  pad_1b5[0x43b];
    s32 unk_5f0;            /* 0x5f0 */
    u8  pad_5f4[0x11];
    u8  unk_605;            /* 0x605 */
    u8  pad_606[0x6];
    s32 unk_60c;            /* 0x60c */
    u8  pad_610[0xc];
    u8  unk_61c;            /* 0x61c */
    u8  unk_61d;            /* 0x61d */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
    int Render();
#endif
};

#endif
