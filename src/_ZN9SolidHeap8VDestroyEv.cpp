//cpp
// @symbol _ZN9SolidHeap8VDestroyEv
/* SolidHeap::VDestroy() at 0x0203c70c -- Heap vtable slot 2. Tear the allocator
 * down and forget it; Heap::Destroy calls this before returning the block to
 * the parent heap.
 *
 * The .c file this replaces was a copy of ExpandingHeap's and never finished
 * being adapted: its comments named ExpandingHeap::VDestroy, quoted
 * ExpandingHeap's address 0x0203c72c instead of this one, and its local struct
 * was called `ExpandingHeap'. Only the symbol on the definition made it a
 * SolidHeap function at all. */
#include "SolidHeap.h"

/* 0x0204ebb8, twelve bytes and still unnamed. It is almost certainly
   SolidHeapAllocator::Destroy -- _ZN13HeapAllocator7DestroyEv at 0x0204e3b4 is
   the same size and shape -- but "almost certainly" is not evidence, and minting
   a mangled symbol is a claim with its own gate (see the naming rule in
   notes/plan-cpp-language-mode.md section 4). The call stays raw until then;
   migration is per-reference, not only per-function. */
extern "C" void func_0204ebb8(SolidHeapAllocator* allocator);

void SolidHeap::VDestroy()
{
    func_0204ebb8(allocator);
    allocator = 0;
}
