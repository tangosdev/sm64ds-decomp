/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class RotatingClockHand: 6 matched functions, 13 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef ROTATINGCLOCKHAND_H
#define ROTATINGCLOCKHAND_H
#include "types.h"

struct RotatingClockHand {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x26];
    s16 unk_08e;            /* 0x08e */
    u8  pad_090[0x2];
    s16 unk_092;            /* 0x092 */
    u8  pad_094[0x40];
    u8  mModel;            /* 0x0d4 */
    u8  pad_0d5[0x4f];
    u8  mMeshCollider;            /* 0x124 */
    u8  pad_125[0x1c7];
    u8  unk_2ec;            /* 0x2ec */
    u8  pad_2ed[0x31];
    s8  unk_31e;            /* 0x31e */
    u8  pad_31f[0x1];
    u16 unk_320;            /* 0x320 */
    u16 unk_322;            /* 0x322 */
    s32 unk_324;            /* 0x324 */
    u8  mShadowModel;            /* 0x328 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
#endif
};

#endif
