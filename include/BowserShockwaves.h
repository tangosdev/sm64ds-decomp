/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class BowserShockwaves: 5 matched functions, 16 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef BOWSERSHOCKWAVES_H
#define BOWSERSHOCKWAVES_H
#include "types.h"

struct BowserShockwaves {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x6c];
    u8  mModelAnim1;            /* 0x0d4 */
    u8  pad_0d5[0x7];
    u8  unk_0dc;            /* 0x0dc */
    u8  pad_0dd[0x47];
    u8  mAnimation1;            /* 0x124 */
    u8  pad_125[0x13];
    u8  mTextureSequence1;            /* 0x138 */
    u8  pad_139[0x13];
    u8  mMaterialChanger1;            /* 0x14c */
    u8  pad_14d[0x13];
    u8  mTextureTransformer1;            /* 0x160 */
    u8  pad_161[0x13];
    u8  mModelAnim2;            /* 0x174 */
    u8  pad_175[0x7];
    u8  unk_17c;            /* 0x17c */
    u8  pad_17d[0x47];
    u8  mAnimation2;            /* 0x1c4 */
    u8  pad_1c5[0x13];
    u8  mTextureSequence2;            /* 0x1d8 */
    u8  pad_1d9[0x13];
    u8  mMaterialChanger2;            /* 0x1ec */
    u8  pad_1ed[0x13];
    u8  mTextureTransformer2;            /* 0x200 */
    u8  pad_201[0x13];
    s16 unk_214;            /* 0x214 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
#endif
};

#endif
