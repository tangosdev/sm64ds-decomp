/* Hand-edited, against evidence. This file used to carry the
 * "AUTO-GENERATED ... by tools/gen_header.py" banner, which stopped being true the
 * moment the __cplusplus block was corrected -- see
 * notes/runbook-type-reconstruction.md section 2.
 *
 * class SolidHeapAllocator: 12 matched functions, 4 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen.
 *
 * THE __cplusplus BLOCK BELOW HAD NEVER BEEN COMPILED. It declared
 *     call ResetEnd void Reset(unsigned int params_);
 * -- `call ResetEnd` is not a type. The generator scraped it out of the comment on
 * Reset's body ("bit 0: call ResetStart; bit 1: call ResetEnd") and spilled it into
 * the return type. Only three files include this header and all three were .c, so
 * the block was always skipped and the syntax error sat there unnoticed. This is the
 * same defect ExpandingHeapAllocator.h carried (`forwards void* Allocate(...)`), from
 * the same generator, found the same way: by migrating a method and compiling it.
 *
 * The generator also emitted `struct align; struct arg_; struct id_; struct params_;
 * struct ptr; struct size_;` as forward declarations -- parameter names mistaken for
 * type names. No source file ever referenced them; they are dropped.
 *
 * STATIC-NESS IS READ OFF THE ARGUMENT COUNT, not the mangled name: the Itanium ABI
 * does not encode `this` in the parameter list, so a static and a non-static member
 * with the same declared parameters mangle identically. AllocateForwards and
 * AllocateBackwards mangle `EPvjj` (three declared parameters) and their ROM bodies
 * take exactly three arguments -- leaving no room for a `this`, so they are static.
 * Their callers confirm it: Allocate and SaveState pass `this + 0x24`, the embedded
 * free-region pair, as that first explicit void*. Every other method's body takes one
 * argument more than it declares, and that extra leading argument is `this`. */
#ifndef SOLIDHEAPALLOCATOR_H
#define SOLIDHEAPALLOCATOR_H
#include "types.h"
#include "HeapAllocator.h"

struct SolidHeapFreeRegion {
    void* begin;
    void* end;
    u32 flags;
};

typedef char SolidHeapFreeRegion_size_must_be_0xc[
    sizeof(struct SolidHeapFreeRegion) == 0xc ? 1 : -1];

struct SolidHeapAllocator : HeapAllocator {
    SolidHeapFreeRegion mFreeRegion; /* 0x024 */
#ifdef __cplusplus
    SolidHeapAllocator(void* heapEnd, u32 flags);

    /* Instance methods. Parameter types are read off the mangled name: `Ej` is
       (unsigned int), `Ei` is (int), `EPvj` is (void*, unsigned int), `Eji` is
       (unsigned int, int), `Ev` is (). */
    void* Allocate(u32 size, int align);
    u32   Reallocate(void* ptr, u32 size);
    int   MemoryLeft(int align);
    s32   TryResizeToFit();
    void  Reset(u32 params);          /* bit 0: ResetStart; bit 1: ResetEnd */
    void  ResetStart();
    void  ResetEnd();
    int   SaveState(u32 arg);
    s32   LoadState(u32 id);

    /* Static: the ROM bodies take no `this`. They operate on the free-region pair
       directly, which is why callers hand them `this + 0x24` rather than `this`.
       `align` here is an unsigned power of two -- Allocate has already taken the
       absolute value and chosen the direction by calling one or the other. */
    static void* AllocateForwards(void* freeBlockPair, u32 size, u32 align);
    static void* AllocateBackwards(void* freeBlockPair, u32 size, u32 align);
#endif
};

typedef char SolidHeapAllocator_size_must_be_0x30[
    sizeof(struct SolidHeapAllocator) == 0x30 ? 1 : -1];

#endif
