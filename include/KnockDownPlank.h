/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class KnockDownPlank: 6 matched functions, 11 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef KNOCKDOWNPLANK_H
#define KNOCKDOWNPLANK_H
#include "types.h"

struct KnockDownPlank {
    u8  pad_000[0xc];
    u8  unk_00c;            /* 0x00c */
    u8  pad_00d[0x4f];
    u8  unk_05c;            /* 0x05c */
    u8  pad_05d[0x3];
    u8  unk_060;            /* 0x060 */
    u8  pad_061[0x3];
    u8  unk_064;            /* 0x064 */
    u8  pad_065[0x29];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x44];
    u8  mModel;            /* 0x0d4 */
    u8  pad_0d5[0x4f];
    u8  mMeshCollider;            /* 0x124 */
    u8  pad_125[0x1fb];
    s32 unk_320;            /* 0x320 */
    s32 unk_324;            /* 0x324 */
    s32 unk_328;            /* 0x328 */
    s32 mVariant;            /* 0x32c */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
#endif
};

#endif
