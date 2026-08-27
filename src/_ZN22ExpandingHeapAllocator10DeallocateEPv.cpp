//cpp
// @symbol _ZN22ExpandingHeapAllocator10DeallocateEPv
#include "ExpandingHeapAllocator.h"

/* ExpandingHeapAllocator::Deallocate(void* ptr) at 0x0204e1a8 -- uses `this`.
 *
 * Frees one block: build a Target describing the node, unlink it from the in-use
 * list, then hand it to FreeNode which merges it back into the free list.
 *
 * The block's MemoryNode header is 0x10 before the user pointer -- note that is a
 * different offset from SizeofInternal's -0xc, because that one reads a field
 * inside the header rather than its base.
 *
 * `inner` is `this + 0x24`, the allocator's embedded node bookkeeping; +8 past it is
 * the in-use list head. Those are raw offsets because the header's padding does not
 * name them yet.
 *
 * The Target local is constructed normally. UnlinkNode and FreeNode keep their raw
 * linkage boundary here until FreeNode itself is declared as a member.
 */

extern "C" {
void _ZN22ExpandingHeapAllocator10UnlinkNodeEP10MemoryNodeS1_(struct MemoryNode *pair, struct MemoryNode *target);
int _ZN22ExpandingHeapAllocator8FreeNodeEP10MemoryNodePNS0_6TargetE(struct MemoryNode *freeNodePair, MemoryNode::Target *target);
}

int ExpandingHeapAllocator::Deallocate(void *ptr)
{
    char *inner;
    struct MemoryNode *node;

    node = (struct MemoryNode *)((char *)ptr - 0x10);
    inner = (char *)this + 0x24;
    MemoryNode::Target target(node);
    _ZN22ExpandingHeapAllocator10UnlinkNodeEP10MemoryNodeS1_((struct MemoryNode *)(inner + 8), node);
    return _ZN22ExpandingHeapAllocator8FreeNodeEP10MemoryNodePNS0_6TargetE((struct MemoryNode *)inner, &target);
}
