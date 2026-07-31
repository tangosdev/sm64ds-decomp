/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Clam: 5 matched functions, 11 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef CLAM_H
#define CLAM_H
#include "types.h"

struct Clam {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x6c];
    u8  mModelAnim;            /* 0x0d4 */
    u8  pad_0d5[0x4f];
    u8  mAnimation;            /* 0x124 */
    u8  pad_125[0x13];
    u8  mMovingCylinderClsn;            /* 0x138 */
    u8  pad_139[0x17];
    u8  unk_150;            /* 0x150 */
    u8  pad_151[0xb];
    s32 unk_15c;            /* 0x15c */
    u8  pad_160[0xc];
    u8  unk_16c;            /* 0x16c */
    u8  pad_16d[0x1];
    u16 mStateTimer;            /* 0x16e */
    u16 unk_170;            /* 0x170 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
    int Render();
#endif
};

#endif
