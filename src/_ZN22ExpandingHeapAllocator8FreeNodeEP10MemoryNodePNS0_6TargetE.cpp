//cpp
// @symbol _ZN22ExpandingHeapAllocator8FreeNodeEP10MemoryNodePNS0_6TargetE
#include "ExpandingHeapAllocator.h"

/* Merge a released extent into the allocator's address-ordered free list.
   The first parameter is the embedded list sentinel, not an allocator object:
   both exact callers pass this+0x24 and provide only this pair of arguments. */
int ExpandingHeapAllocator::FreeNode(
    MemoryNode *list,
    MemoryNode::Target *extent)
{
    struct TargetWords { u32 words[2]; };
    MemoryNode::Target merged;

    *(TargetWords *)&merged = *(TargetWords *)extent;

    MemoryNode *below = 0;
    MemoryNode *node = *(MemoryNode **)list;

    while (node != 0) {
        if ((u32)node < (u32)extent->start) {
            below = node;
        } else {
            if ((u32)node == (u32)extent->end) {
                merged.end = (char *)(node->size + ((u32)node + 0x10));
                UnlinkNode(list, node);
            }
            break;
        }
        node = node->next;
    }

    if (below != 0) {
        u32 belowEnd = below->size + ((u32)below + 0x10);
        if ((char *)belowEnd == extent->start) {
            merged.start = (char *)below;
            below = (MemoryNode *)UnlinkNode(list, below);
        }
    }

    if ((u32)(merged.end - merged.start) < 0x10)
        return 0;

    MemoryNode *node_ = (MemoryNode *)CreateNode(&merged, 0x4652);
    LinkNode(list, node_, below);
    return 1;
}
