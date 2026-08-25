/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class HeapAllocator: 2 matched functions, 3 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef HEAPALLOCATOR_H
#define HEAPALLOCATOR_H
#include "types.h"

struct HeapAllocator {
    u8  pad_000[0x18];
    /* Pointers, not integers -- the constructor spells it:
       src/_ZN13HeapAllocatorC1EjPvPvj.cpp stores its two `void*` parameters here as
       `*(void**)((char*)&self->unk_018) = a;`. Same 4 bytes as s32, so this is
       byte-identical; it just stops the header under-claiming what the tree proves. */
    void *mStart;              /* 0x018 */
    void *mEnd;              /* 0x01c */
    u32 unk_020;            /* 0x020 */
#ifdef __cplusplus
    /* methods */
    void Remove();
#endif
};

#endif
