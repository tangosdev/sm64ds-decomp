#include "types.h"
// @symbol _ZN22ExpandingHeapAllocator14SizeofInternalEPv
/* recovered: named members + shared header */
#include "ExpandingHeapAllocator.h"
/* ExpandingHeapAllocator::SizeofInternal(void* userPtr) at 0x0204e084
 * Returns the allocated size of a block given a user pointer. The block's
 * MemoryNode header sits immediately before the user data; its `size` field (see
 * MemoryNode in Memory.h) lives 3 words (0xc bytes) ahead of the user pointer,
 * i.e. ((s32*)userPtr)[-3]. */
u32 _ZN22ExpandingHeapAllocator14SizeofInternalEPv(void *userPtr)
{
    return ((s32 *)userPtr)[-3]; /* MemoryNode.size, header at userPtr - 0xc */
}
