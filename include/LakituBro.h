/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class LakituBro: 5 matched functions, 9 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef LAKITUBRO_H
#define LAKITUBRO_H
#include "types.h"

struct LakituBro {
    u8  pad_000[0x8];
    s32 unk_008;            /* 0x008 */
    u8  pad_00c[0x104];
    u8  mModelAnim1;            /* 0x110 */
    u8  pad_111[0x5b];
    s32 unk_16c;            /* 0x16c */
    u8  pad_170[0x4];
    u8  mModelAnim2;            /* 0x174 */
    u8  pad_175[0x63];
    u8  mTextureSequence;            /* 0x1d8 */
    u8  pad_1d9[0x17];
    u8  mShadowModel1;            /* 0x1f0 */
    u8  pad_1f1[0x27];
    u8  mShadowModel2;            /* 0x218 */
    u8  pad_219[0xb7];
    s32 unk_2d0;            /* 0x2d0 */
    u8  pad_2d4[0x8];
    u8  unk_2dc;            /* 0x2dc */
#ifdef __cplusplus
    /* methods */
    int CleanupResources();
    void OnPendingDestroy();
    int Behavior();
    int InitResources();
    int Render();
#endif
};

#endif
