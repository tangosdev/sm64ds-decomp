/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class ExpandingHeap: 10 matched functions, 1 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef EXPANDINGHEAP_H
#define EXPANDINGHEAP_H
#include "types.h"

/* ExpandingHeapAllocator is only ever pointed at from here, so a declaration is enough --
 * no definition is pulled in. The typedef keeps the member spelled the
 * same in C and in C++; the guard is common.h's idiom for the same job. */
#ifndef EXPANDINGHEAPALLOCATOR_FWD_DECLARED
#define EXPANDINGHEAPALLOCATOR_FWD_DECLARED
struct ExpandingHeapAllocator;
typedef struct ExpandingHeapAllocator ExpandingHeapAllocator;
#endif

struct ExpandingHeap {
    u8  pad_000[0x14];
    /* ExpandingHeapAllocator * -- the ROM loads this WORD and passes it to
       _ZN22ExpandingHeapAllocator18MaxAllocatableSizeEi as that function's `this`, which
       is an object address, so the word is a ExpandingHeapAllocator *. It says nothing
       about the rest of the marker's span, which stays explicit padding. Was a u8 marker. */
    ExpandingHeapAllocator *unk_014;            /* 0x014 */
#ifdef __cplusplus
    /* methods */
    bool VIntact();
    u32 VResizeToFit();
    unsigned int VMaxAllocatableSize();
    unsigned int VMaxAllocationUnitSize();
    void VRescue();
#endif
};

#endif
