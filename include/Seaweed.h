/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Seaweed: 5 matched functions, 9 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef SEAWEED_H
#define SEAWEED_H
#include "types.h"

struct Seaweed {
    u8  pad_000[0x74];
    u8  unk_074;            /* 0x074 */
    u8  pad_075[0x5f];
    u8  mModelAnim;            /* 0x0d4 */
    u8  pad_0d5[0x4f];
    u8  mAnimation;            /* 0x124 */
    u8  pad_125[0xb];
    s32 unk_130;            /* 0x130 */
    u8  pad_134[0x4];
    u8  mMovingCylinderClsn;            /* 0x138 */
    u8  pad_139[0x23];
    s32 unk_15c;            /* 0x15c */
    u8  pad_160[0xc];
    s32 unk_16c;            /* 0x16c */
    u8  mHealTimer;            /* 0x170 */
    u8  unk_171;            /* 0x171 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
    int Render();
#endif
};

#endif
