/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class FirePiranhaPlantBig: 6 matched functions, 29 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef FIREPIRANHAPLANTBIG_H
#define FIREPIRANHAPLANTBIG_H
#include "types.h"

struct FirePiranhaPlantBig {
    u8  pad_000[0x8];
    s32 unk_008;            /* 0x008 */
    u16 unk_00c;            /* 0x00c */
    u8  pad_00e[0xa2];
    s32 unk_0b0;            /* 0x0b0 */
    u8  pad_0b4[0x5c];
    u8  mModelAnim;            /* 0x110 */
    u8  pad_111[0x4f];
    u8  mAnimation;            /* 0x160 */
    u8  pad_161[0x13];
    u8  mMovingCylinderClsn;            /* 0x174 */
    u8  pad_175[0x3];
    s32 unk_178;            /* 0x178 */
    s32 unk_17c;            /* 0x17c */
    u8  pad_180[0x28];
    u8  mMovingCylinderClsnWithPos;            /* 0x1a8 */
    u8  pad_1a9[0x3];
    s32 unk_1ac;            /* 0x1ac */
    s32 unk_1b0;            /* 0x1b0 */
    u8  pad_1b4[0x34];
    s32 unk_1e8;            /* 0x1e8 */
    s32 mState;            /* 0x1ec */
    s32 unk_1f0;            /* 0x1f0 */
    s32 unk_1f4;            /* 0x1f4 */
    u8  pad_1f8[0xc];
    s32 mScale;            /* 0x204 */
    s32 unk_208;            /* 0x208 */
    s32 unk_20c;            /* 0x20c */
    s32 unk_210;            /* 0x210 */
    s32 unk_214;            /* 0x214 */
    u8  pad_218[0x2];
    u8  unk_21a;            /* 0x21a */
    u8  unk_21b;            /* 0x21b */
    u8  unk_21c;            /* 0x21c */
    u8  unk_21d;            /* 0x21d */
    u8  unk_21e;            /* 0x21e */
    u8  unk_21f;            /* 0x21f */
    u8  unk_220;            /* 0x220 */
    u8  pad_221[0x3];
    s32 unk_224;            /* 0x224 */
    s32 unk_228;            /* 0x228 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
#endif
};

#endif
