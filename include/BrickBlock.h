/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class BrickBlock: 5 matched functions, 7 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef BRICKBLOCK_H
#define BRICKBLOCK_H
#include "types.h"

struct BrickBlock {
    u8  pad_000[0x8];
    s32 param1;            /* 0x008 */
    u16 actorID;            /* 0x00c */
    u8  pad_00e[0xc6];
    s8  unk_0d4;            /* 0x0d4 */
    u8  unk_0d5;            /* 0x0d5 */
    u8  unk_0d6;            /* 0x0d6 */
    u8  unk_0d7;            /* 0x0d7 */
    u8  unk_0d8;            /* 0x0d8 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
#endif
};

typedef char BrickBlock_size_must_be_0xdc[sizeof(struct BrickBlock) == 0xdc ? 1 : -1];

#endif
