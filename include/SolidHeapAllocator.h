/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class SolidHeapAllocator: 12 matched functions, 4 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef SOLIDHEAPALLOCATOR_H
#define SOLIDHEAPALLOCATOR_H
#include "types.h"

/* fwd */
struct align;
struct arg_;
struct id_;
struct params_;
struct ptr;
struct size_;
struct SolidHeapAllocator {
    u8  pad_000[0x18];
    s32 unk_018;            /* 0x018 */
    s32 mEnd;            /* 0x01c */
    s32 unk_020;            /* 0x020 */
    u8  mFreeRegion;            /* 0x024 */
#ifdef __cplusplus
    /* methods */
    call ResetEnd void Reset(unsigned int params_);
    int SaveState(unsigned int arg_);
    s32 LoadState(unsigned int id_);
    s32 TryResizeToFit();
    u32 Reallocate(void * ptr, unsigned int size_);
    void ResetEnd();
    void ResetStart();
    void* Allocate(unsigned int size_, int align);
#endif
};

#endif
