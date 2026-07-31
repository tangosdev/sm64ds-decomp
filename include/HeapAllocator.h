/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class HeapAllocator: 2 matched functions, 3 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef HEAPALLOCATOR_H
#define HEAPALLOCATOR_H
#include "types.h"

struct HeapAllocator {
    u8  pad_000[0x18];
    u8  unk_018;            /* 0x018 */
    u8  pad_019[0x3];
    u8  unk_01c;            /* 0x01c */
    u8  pad_01d[0x3];
    u32 unk_020;            /* 0x020 */
#ifdef __cplusplus
    /* methods */
    void Remove();
#endif
};

#endif
