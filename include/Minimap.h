/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Minimap: 9 matched functions, 25 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef MINIMAP_H
#define MINIMAP_H
#include "types.h"

struct Minimap {
    u8  pad_000[0x50];
    s32 unk_050;            /* 0x050 */
    s32 unk_054;            /* 0x054 */
    s32 unk_058;            /* 0x058 */
    s32 mPosX;            /* 0x05c */
    u8  pad_060[0x30];
    s32 unk_090;            /* 0x090 */
    s32 unk_094;            /* 0x094 */
    s32 unk_098;            /* 0x098 */
    s32 unk_09c;            /* 0x09c */
    u8  pad_0a0[0x134];
    s32 unk_1d4;            /* 0x1d4 */
    s32 unk_1d8;            /* 0x1d8 */
    s32 unk_1dc;            /* 0x1dc */
    s32 unk_1e0;            /* 0x1e0 */
    s32 unk_1e4;            /* 0x1e4 */
    s32 unk_1e8;            /* 0x1e8 */
    s32 unk_1ec;            /* 0x1ec */
    u8  pad_1f0[0x10];
    s32 unk_200;            /* 0x200 */
    s32 unk_204;            /* 0x204 */
    s32 unk_208;            /* 0x208 */
    s32 unk_20c;            /* 0x20c */
    s32 unk_210;            /* 0x210 */
    s32 unk_214;            /* 0x214 */
    s32 unk_218;            /* 0x218 */
    u16 unk_21c;            /* 0x21c */
    u8  pad_21e[0x33];
    u8  unk_251;            /* 0x251 */
    u8  pad_252[0x3];
    u8  unk_255;            /* 0x255 */
#ifdef __cplusplus
    /* methods */
    int InitResources();
    s32 CleanupResources();
    void OnPendingDestroy();
#endif
};

#endif
