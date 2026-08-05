/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class ExpandingHeapAllocator: 18 matched functions, 7 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef EXPANDINGHEAPALLOCATOR_H
#define EXPANDINGHEAPALLOCATOR_H
#include "types.h"

/* fwd */
struct align_;
struct ptr;
struct size_;
struct ExpandingHeapAllocator {
    u8  pad_000[0xc];
    s32 unk_00c;            /* 0x00c */
    u8  unk_010;            /* 0x010 */
    u8  pad_011[0x7];
    s32 unk_018;            /* 0x018 */
    s32 unk_01c;            /* 0x01c */
    s32 unk_020;            /* 0x020 */
    u8  unk_024;            /* 0x024 */
    u8  pad_025[0x7];
    u8  unk_02c;            /* 0x02c */
#ifdef __cplusplus
    /* methods */
    forwards void* Allocate(unsigned int size_, int align_);
    int Deallocate(void * ptr);
    u32 GetNodeID();
#endif
};

#endif
