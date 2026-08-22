/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Tree: 5 matched functions, 4 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef TREE_H
#define TREE_H
#include "types.h"

struct Tree {
    u8  pad_000[0x8];
    s32 mParam;            /* 0x008 */
    u8  pad_00c[0x50];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    /* trailing extent the ROM's `new Tree` literal proves; see tools/opnew_sizes.py */
    u8 pad_068[0x1fc];
#ifdef __cplusplus
    /* methods */
    int CleanupResources();
    int Render();
#endif
};

typedef char Tree_size_must_be_0x264[sizeof(struct Tree) == 0x264 ? 1 : -1];

#endif
