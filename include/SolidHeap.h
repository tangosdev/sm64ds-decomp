/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class SolidHeap: 15 matched functions, 1 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef SOLIDHEAP_H
#define SOLIDHEAP_H
#include "types.h"

/* fwd */
struct a;
struct id_;
struct SolidHeap {
    u8  pad_000[0x14];
    u8  unk_014;            /* 0x014 */
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
