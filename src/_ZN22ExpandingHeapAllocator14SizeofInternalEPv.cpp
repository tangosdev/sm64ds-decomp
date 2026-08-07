//cpp
// @symbol _ZN22ExpandingHeapAllocator14SizeofInternalEPv
#include "ExpandingHeapAllocator.h"

/* ExpandingHeapAllocator::SizeofInternal(void* userPtr) at 0x0204e084 -- static.
 *
 * Returns the allocated size of a block from a user pointer alone, which is why it
 * needs no allocator instance: every block carries a MemoryNode header immediately
 * before its user data, and the `size` field sits three words (0xc) back.
 */
u32 ExpandingHeapAllocator::SizeofInternal(void *userPtr)
{
    return ((s32 *)userPtr)[-3]; /* MemoryNode.size, header at userPtr - 0xc */
}
