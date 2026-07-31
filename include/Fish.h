/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Fish: 6 matched functions, 11 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef FISH_H
#define FISH_H
#include "types.h"

struct Fish {
    u8  pad_000[0x4];
    s32 unk_004;            /* 0x004 */
    u32 mParam;            /* 0x008 */
    u8  pad_00c[0x54];
    s32 mPosY;            /* 0x060 */
    u8  pad_064[0x70];
    u8  mModelAnim;            /* 0x0d4 */
    u8  pad_0d5[0x67];
    s32 unk_13c;            /* 0x13c */
    s32 unk_140;            /* 0x140 */
    u8  pad_144[0x8];
    s32 unk_14c;            /* 0x14c */
    s32 unk_150;            /* 0x150 */
    u8  pad_154[0x5];
    u8  unk_159;            /* 0x159 */
    u8  pad_15a[0x1];
    u8  mModelIndex;            /* 0x15b */
    u8  unk_15c;            /* 0x15c */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
#endif
};

#endif
