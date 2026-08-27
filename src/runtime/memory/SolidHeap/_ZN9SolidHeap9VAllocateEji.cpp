//cpp
// @symbol _ZN9SolidHeap9VAllocateEji
/* SolidHeap::VAllocate(u32, int) at 0x0203c6ac -- Heap vtable slot 3. Four
 * instructions: load the allocator from this+0x14 and tail-call it, passing
 * r1 and r2 through untouched.
 *
 * THE SYMBOL WAS _ZN9SolidHeap9VAllocateEjj AND THAT WAS WRONG. `Ejj' claims
 * (u32, u32); ExpandingHeap's override of the SAME SLOT is
 * _ZN13ExpandingHeap9VAllocateEji, which claims (u32, int). Two overrides of
 * one slot cannot have different parameter types, so one import was wrong, and
 * nothing in the tree could tell which -- a caller cannot observe a parameter's
 * signedness through a virtual call, and until SolidHeap actually derived from
 * Heap nothing compiled the two declarations against each other.
 *
 * The ROM decides it. This function forwards r2 straight into
 * SolidHeapAllocator::Allocate (0x0204eb70), which does:
 *
 *     cmp r2, #0
 *     blt 0x0204eba4        <- SIGNED branch
 *     ...
 *     0x0204eba4: rsb r2, r2, #0    <- negate, then allocate backwards
 *
 * `blt' is signed. Declared u32 the compiler emits `bcc'/`blo' and the
 * negate-and-allocate-backwards path becomes unreachable, so the alignment
 * argument is an `int' whose sign selects the direction. Every sibling in the
 * family already spelled it that way: Heap::Allocate,
 * SolidHeapAllocator::Allocate and ExpandingHeapAllocator::Allocate are all
 * `Eji'. The symbol is renamed in config/arm9/symbols.txt to match, and this
 * file with it.
 *
 * It also carried a local `class SolidHeap { u32 unk00; ... }' rather than
 * including the header. */
#include "SolidHeap.h"
#include "SolidHeapAllocator.h"

void* SolidHeap::VAllocate(u32 size, int align)
{
    return allocator->Allocate(size, align);
}
