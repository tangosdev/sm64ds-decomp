//cpp
// @symbol _ZN4Heap10DeallocateEPv
/* Heap::Deallocate(void*) at 0x0203c538 -- forwards to the concrete heap's
 * slot 4. No guard: freeing is the one operation with nothing to fail at.
 *
 * Was a cast of `this' to a local `struct Base' with four anonymous virtuals
 * padding `m' out to index 4; VDeallocate is now Heap's own slot-4 pure
 * virtual. */
#include "Heap.h"

void Heap::Deallocate(void* ptr)
{
    VDeallocate(ptr);
}
