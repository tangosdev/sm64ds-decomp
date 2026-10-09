//cpp
// @symbol _ZN9SolidHeap7VSizeofEPv
/* SolidHeap::VSizeof(void*) at 0x0203c428 -- Heap vtable slot 9.
 *
 * A linear allocator does not record block sizes, so asking is a programming
 * error: it calls Crash() and, if that ever returns, reports -1 -- the failure
 * value Heap::Sizeof tests for.
 *
 * Return type widened from int to u32 to match the slot. Byte-unobservable:
 * both spellings lower the -1 to `mvn r0,#0'. It is a consistency change, not
 * evidence of a width. */
#include "decl_common.h"
#include "SolidHeap.h"

u32 SolidHeap::VSizeof(void* ptr)
{
    Crash();
    return (u32)-1;
}
