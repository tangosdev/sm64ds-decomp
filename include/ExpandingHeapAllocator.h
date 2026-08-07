/* Hand-edited, against evidence. This file used to carry the
 * "AUTO-GENERATED ... by tools/gen_header.py" banner, which was never true --
 * see notes/runbook-type-reconstruction.md section 2.
 *
 * class ExpandingHeapAllocator: 18 matched functions, 7 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen.
 *
 * THE __cplusplus BLOCK BELOW HAD NEVER BEEN COMPILED. It declared
 * `forwards void* Allocate(...)` -- `forwards` is not a keyword, and the generator
 * evidently spilled a fragment of a parameter name into the return type. Only two
 * files include this header and both were .c, so the block was always skipped and
 * the syntax error sat there unnoticed. Migrating the first method to C++ is what
 * compiled it for the first time. */
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
    /* methods. Parameter types are read off the mangled name: `Eji` is
       (unsigned int, int), `EPv` is (void*), `Ev` is (). */
    void* Allocate(u32 size, int align);
    int Deallocate(void* ptr);
    u32 GetNodeID();

    /* Static: the ROM body takes no `this`. SizeofInternal reads the block's
       MemoryNode header, which sits immediately before the user pointer, so it
       needs no allocator instance. */
    static u32 SizeofInternal(void* userPtr);
#endif
};

#endif
