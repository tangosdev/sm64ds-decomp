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
 * The three callees keep their raw mangled names: MemoryNode::Target's constructor
 * is a C1 (Phase 5, never migrated), and UnlinkNode/FreeNode take MemoryNode and
 * MemoryNode::Target parameters this header cannot spell without dragging in
 * Memory.h's own reconstruction. Migration is per-reference.
 */
struct MemoryNodeTarget {
    void *start;
    void *end;
};

/* MemoryNode now comes from include/MemoryNode.h. The local copy this file carried
   spelled offset 0 as `char magic[2]`; the shared type calls it a u16 tag, which is
   what CreateNode stores. Nothing here read the field. */

extern "C" {
void _ZN10MemoryNode6TargetC1EP10MemoryNode(struct MemoryNodeTarget *t, struct MemoryNode *node);
void _ZN22ExpandingHeapAllocator10UnlinkNodeEP10MemoryNodeS1_(struct MemoryNode *pair, struct MemoryNode *target);
int _ZN22ExpandingHeapAllocator8FreeNodeEP10MemoryNodePNS0_6TargetE(struct MemoryNode *freeNodePair, struct MemoryNodeTarget *target);
}

int ExpandingHeapAllocator::Deallocate(void *ptr)
{
    struct MemoryNodeTarget target;
    char *inner;
    struct MemoryNode *node;

    node = (struct MemoryNode *)((char *)ptr - 0x10);
    inner = (char *)this + 0x24;
    _ZN10MemoryNode6TargetC1EP10MemoryNode(&target, node);
    _ZN22ExpandingHeapAllocator10UnlinkNodeEP10MemoryNodeS1_((struct MemoryNode *)(inner + 8), node);
    return _ZN22ExpandingHeapAllocator8FreeNodeEP10MemoryNodePNS0_6TargetE((struct MemoryNode *)inner, &target);
}
