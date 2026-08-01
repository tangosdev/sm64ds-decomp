/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class ExpandingHeap: 10 matched functions, 1 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef EXPANDINGHEAP_H
#define EXPANDINGHEAP_H
#include "types.h"

struct ExpandingHeap {
    u8  pad_000[0x14];
    u8  unk_014;            /* 0x014 */
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
