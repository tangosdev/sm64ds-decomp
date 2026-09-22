//cpp
// @symbol _ZN22ExpandingHeapAllocator16InvokeDeallocateEPvPS_j
#include "ExpandingHeapAllocator.h"

/* ExpandingHeapAllocator::InvokeDeallocate(void*, ExpandingHeapAllocator*, u32)
 * at 0x0203c4cc -- STATIC.
 *
 * The mangled name declares three parameters (`EPvPS_j`, where `S_` is a substitution
 * for ExpandingHeapAllocator itself) and the ROM body takes exactly three arguments,
 * leaving no room for a `this`. It is the default trampoline DeallocateAll hands each
 * live block to, so it has to match that callback's shape -- (block, allocator, arg)
 * -- which is exactly why the allocator arrives as an ordinary parameter.
 *
 * It ignores `size` and forwards to the ordinary Deallocate. Note the argument order
 * flips: the callback receives (block, allocator) and the method needs (allocator,
 * block).
 *
 * Deallocate is now a real member call, replacing the hand-spelled
 * `extern void _ZN22ExpandingHeapAllocator10DeallocateEPv(void*, void*)` -- which in a
 * C++ TU would have mangled a second time.
 */
void ExpandingHeapAllocator::InvokeDeallocate(void* p, ExpandingHeapAllocator* alloc, u32 size)
{
    alloc->Deallocate(p);
}
