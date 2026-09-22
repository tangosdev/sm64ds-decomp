//cpp
// @symbol _ZN18SolidHeapAllocator10ResetStartEv
#include "SolidHeapAllocator.h"

/* SolidHeapAllocator::ResetStart() at 0x0204ec40 -- uses `this`.
 *
 * Rewinds the free region's begin pointer back to the heap's start (+0x18) and clears
 * its flags, discarding every forward allocation at once. ResetEnd is the mirror.
 *
 * The `(long long)(int)` round-trip on the store is deliberate and load-bearing: it
 * is what makes mwccarm materialise the address in the register the ROM uses instead
 * of folding the +0x24 into the store's offset. Writing `fl->begin = begin;` compiles
 * to different bytes. Left exactly as recovered.
 */
struct FreeList { void *begin; void *end; int flags; };

void SolidHeapAllocator::ResetStart()
{
    void *begin;
    struct FreeList *fl;

    begin = *(void **)((char *)this + 0x18);
    fl = (struct FreeList *)((char *)this + 0x24);
    *(void **)(fl) = begin;
    fl->flags = 0;
}
