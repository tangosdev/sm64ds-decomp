/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class MirrorLuigi: 5 matched functions, 16 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef MIRRORLUIGI_H
#define MIRRORLUIGI_H
#include "types.h"

struct MirrorLuigi {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x6c];
    u8  mModelAnim;            /* 0x0d4 */
    u8  pad_0d5[0x7];
    u8  unk_0dc;            /* 0x0dc */
    u8  pad_0dd[0xb];
    u8  unk_0e8;            /* 0x0e8 */
    u8  pad_0e9[0x4f];
    u8  mModel;            /* 0x138 */
    u8  pad_139[0x7];
    u8  unk_140;            /* 0x140 */
    u8  pad_141[0xb];
    u8  unk_14c;            /* 0x14c */
    u8  pad_14d[0x7];
    u8  unk_154;            /* 0x154 */
    u8  pad_155[0x33];
    u8  mShadowModel;            /* 0x188 */
    u8  pad_189[0x27];
    u8  unk_1b0;            /* 0x1b0 */
    u8  pad_1b1[0x7];
    s32 unk_1b8;            /* 0x1b8 */
    u8  pad_1bc[0x8];
    u8  unk_1c4;            /* 0x1c4 */
    u8  pad_1c5[0x7];
    s32 unk_1cc;            /* 0x1cc */
    u8  pad_1d0[0x8];
    u8  unk_1d8;            /* 0x1d8 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
    int Render();
#endif
};

#endif
