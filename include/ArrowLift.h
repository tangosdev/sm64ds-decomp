/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class ArrowLift: 5 matched functions, 8 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef ARROWLIFT_H
#define ARROWLIFT_H
#include "types.h"

struct ArrowLift {
    u8  pad_000[0x8];
    s32 mParam;            /* 0x008 */
    u8  pad_00c[0x54];
    s32 mPosY;            /* 0x060 */
    u8  pad_064[0x2a];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x44];
    u8  mModel;            /* 0x0d4 */
    u8  pad_0d5[0x4f];
    u8  mMovingCylinderClsn;            /* 0x124 */
    u8  pad_125[0x33];
    s32 unk_158;            /* 0x158 */
    s8  unk_15c;            /* 0x15c */
    u8  unk_15d;            /* 0x15d */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
    int Render();
#endif
};

#endif
