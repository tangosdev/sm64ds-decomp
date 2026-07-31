/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class NestedHeapIterator: 10 matched functions, 3 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef NESTEDHEAPITERATOR_H
#define NESTEDHEAPITERATOR_H
#include "types.h"

/* fwd */
struct HeapAllocator;
struct a_;
struct addr;
struct h_;
struct node_;
struct NestedHeapIterator {
    u8  pad_000[0x4];
    s32 mLast;            /* 0x004 */
    u8  unk_008;            /* 0x008 */
    u8  pad_009[0x1];
    u16 mLinkOffset;            /* 0x00a */
#ifdef __cplusplus
    /* methods */
    int Next(HeapAllocator * h_);
    int Previous(HeapAllocator * h_);
    void AddFirst(HeapAllocator * a_);
    void AddLast(HeapAllocator * a_);
    void Init(HeapAllocator * a_);
    void Remove(HeapAllocator * node_);
    void* RecursiveFindNested(void * addr);
#endif
};

#endif
