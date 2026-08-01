/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class BobOmbBuddy: 5 matched functions, 11 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef BOBOMBBUDDY_H
#define BOBOMBBUDDY_H
#include "types.h"

struct BobOmbBuddy {
    u8  pad_000[0x8];
    s32 mParam;            /* 0x008 */
    u8  pad_00c[0x50];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x26];
    u8  unk_08e;            /* 0x08e */
    u8  pad_08f[0x45];
    u8  mMovingCylinderClsn;            /* 0x0d4 */
    u8  pad_0d5[0x33];
    u8  mModelAnim;            /* 0x108 */
    u8  pad_109[0x4f];
    u8  mAnimation;            /* 0x158 */
    u8  pad_159[0x7];
    s32 unk_160;            /* 0x160 */
    u8  pad_164[0x8];
    u8  mShadowModel;            /* 0x16c */
    u8  pad_16d[0x2b];
    s32 unk_198;            /* 0x198 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
    int Render();
#endif
};

#endif
