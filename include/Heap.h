/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Heap: 21 matched functions, 4 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef HEAP_H
#define HEAP_H
#include "types.h"

/* fwd */
struct a;
struct a_;
struct b;
struct Heap {
    u8  pad_000[0x4];
    s32 unk_004;            /* 0x004 */
    s32 unk_008;            /* 0x008 */
    s32 unk_00c;            /* 0x00c */
    u8  unk_010;            /* 0x010 */
#ifdef __cplusplus
    /* methods */
    bool Intact();
    int Allocate(unsigned int a, int b);
    int MaxAllocationUnitSize();
    int Rescue();
    int SetDefault();
    int Sizeof(void * a);
    void Deallocate(void * a);
    void Destroy();
    void Reallocate(void * a, unsigned int b);
    void SetNodeID(unsigned int a_);
#endif
};

#endif
