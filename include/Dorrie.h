/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Dorrie: 5 matched functions, 27 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef DORRIE_H
#define DORRIE_H
#include "types.h"

struct Dorrie {
    u8  pad_000[0x8];
    s32 mParam;            /* 0x008 */
    u8  pad_00c[0x50];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x26];
    s16 unk_08e;            /* 0x08e */
    u8  pad_090[0x3c];
    s8  mAreaId;            /* 0x0cc */
    u8  pad_0cd[0x7];
    s32 unk_0d4;            /* 0x0d4 */
    s32 unk_0d8;            /* 0x0d8 */
    s32 unk_0dc;            /* 0x0dc */
    s32 unk_0e0;            /* 0x0e0 */
    u8  pad_0e4[0x4];
    u8  unk_0e8;            /* 0x0e8 */
    u8  pad_0e9[0x3];
    u8  mModelAnim;            /* 0x0ec */
    u8  pad_0ed[0xe63];
    u8  mWithMeshClsn;            /* 0xf50 */
    u8  pad_f51[0x1bb];
    u8  unk_110c;           /* 0x110c */
    u8  pad_110d[0x33];
    u8  unk_1140;           /* 0x1140 */
    u8  pad_1141[0x3f];
    s32 unk_1180;           /* 0x1180 */
    s32 unk_1184;           /* 0x1184 */
    s32 unk_1188;           /* 0x1188 */
    s32 unk_118c;           /* 0x118c */
    s32 unk_1190;           /* 0x1190 */
    s32 unk_1194;           /* 0x1194 */
    s32 unk_1198;           /* 0x1198 */
    s32 unk_119c;           /* 0x119c */
    u8  pad_11a0[0x8];
    s32 unk_11a8;           /* 0x11a8 */
    s32 unk_11ac;           /* 0x11ac */
    u8  pad_11b0[0x2];
    s16 unk_11b2;           /* 0x11b2 */
    u8  pad_11b4[0x1];
    u8  unk_11b5;           /* 0x11b5 */
#ifdef __cplusplus
    /* methods */
    int CleanupResources();
    int InitResources();
    int Render();
#endif
};

#endif
