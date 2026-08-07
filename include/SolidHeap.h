/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class SolidHeap: 15 matched functions, 1 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef SOLIDHEAP_H
#define SOLIDHEAP_H
#include "types.h"

/* SolidHeapAllocator is only ever pointed at from here, so a declaration is enough --
 * no definition is pulled in. The typedef keeps the member spelled the
 * same in C and in C++; the guard is common.h's idiom for the same job. */
#ifndef SOLIDHEAPALLOCATOR_FWD_DECLARED
#define SOLIDHEAPALLOCATOR_FWD_DECLARED
struct SolidHeapAllocator;
typedef struct SolidHeapAllocator SolidHeapAllocator;
#endif

/* fwd */
struct a;
struct id_;
struct SolidHeap {
    u8  pad_000[0x14];
    /* SolidHeapAllocator * -- the ROM loads this WORD and passes it to
       _ZN18SolidHeapAllocator10MemoryLeftEi as that function's `this`, which is an object
       address, so the word is a SolidHeapAllocator *. It says nothing about the rest of
       the marker's span, which stays explicit padding. Was a u8 marker. */
    SolidHeapAllocator *unk_014;            /* 0x014 */
#ifdef __cplusplus
    /* methods */
    bool VIntact();
    int VSizeof(void * a);
    u32 VGetNodeID();
    u32 VSetNodeID(unsigned int id_);
    unsigned int VMaxAllocatableSize();
    unsigned int VMaxAllocationUnitSize();
    unsigned int VMemoryLeft();
    void VDeallocate(void * a);
    void VDeallocateAll();
    void VRescue();
#endif
};

#endif
