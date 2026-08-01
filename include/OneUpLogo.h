/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class OneUpLogo: 5 matched functions, 17 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef ONEUPLOGO_H
#define ONEUPLOGO_H
#include "types.h"

struct OneUpLogo {
    u8  pad_000[0x8];
    u32 mParam;            /* 0x008 */
    u8  pad_00c[0x50];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x34];
    s32 unk_09c;            /* 0x09c */
    s32 unk_0a0;            /* 0x0a0 */
    u8  pad_0a4[0x4];
    s32 unk_0a8;            /* 0x0a8 */
    u8  pad_0ac[0x28];
    u8  mModel;            /* 0x0d4 */
    u8  pad_0d5[0x1b];
    u8  unk_0f0;            /* 0x0f0 */
    u8  pad_0f1[0x33];
    u8  mTextureSequence;            /* 0x124 */
    u8  pad_125[0x13];
    s32 unk_138;            /* 0x138 */
    s32 unk_13c;            /* 0x13c */
    s32 unk_140;            /* 0x140 */
    s32 unk_144;            /* 0x144 */
    s32 unk_148;            /* 0x148 */
    s16 unk_14c;            /* 0x14c */
    u8  unk_14e;            /* 0x14e */
#ifdef __cplusplus
    /* methods */
    int InitResources();
    int Render();
#endif
};

#endif
