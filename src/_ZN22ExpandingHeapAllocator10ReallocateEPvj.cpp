//cpp
#include "ExpandingHeapAllocator.h"
/* ExpandingHeapAllocator::Reallocate(void* ptr, u32 size) at 0x0204e1e8
 * Grows a block in place by swallowing the free node that directly follows it,
 * or shrinks it and hands the tail back to the free list.
 *
 * The user pointer sits 0x10 bytes past its MemoryNode header; the allocator's
 * free-node list head lives at this+0x24, and this+0x20 holds the flag word whose
 * low bit asks for freshly-gained bytes to be zero filled. */
extern "C" {
void _ZN10MemoryNode6TargetC1EPS_(struct MemoryNode::Target* t, struct MemoryNode* node);
void MultiStore_Int(int val, void* dst, int len);
int _ZN22ExpandingHeapAllocator8FreeNodeEP10MemoryNodePNS0_6TargetE(void* list, struct MemoryNode::Target* t);
}

static inline struct MemoryNode** GetList(void* t) { return (struct MemoryNode**)((char*)t + 0x24); }

u32 ExpandingHeapAllocator::Reallocate(void* ptrRaw, u32 size)
{
    struct MemoryNode::Target tgt;
    struct MemoryNode::Target tgt2;
    volatile int fill;
    struct MemoryNode* block;
    u32 blockSize;
    struct MemoryNode* node;
    struct MemoryNode** list;
    char* ptr = (char*)ptrRaw;

    node = (struct MemoryNode*)ptr;
    node = (struct MemoryNode*)((char*)node - 0x10);
    list = GetList(this);
    blockSize = node->size;
    size = (size + 3) & ~3u;

    if (size == blockSize) {
        return size;
    }

    if (size > blockSize) {
        char* end = (char*)(blockSize + ((u32)node + 0x10));
        char* oldStart;
        struct MemoryNode* prev;

        /* the block right after ours has to be free for the grow to work */
        block = *list;
        while (block != 0) {
            if (block == (struct MemoryNode*)end) break;
            block = block->next;
        }
        if (block == 0 || size > blockSize + 0x10 + block->size) {
            return 0;
        }

        /* The target must retain its function-scope stack slot. Declaring it at
         * this point changes mwccarm's slot coloring, while this compiler cannot
         * express placement new; keep the single typed constructor ABI seam. */
        _ZN10MemoryNode6TargetC1EPS_(&tgt, block);
        prev = (MemoryNode*)UnlinkNode((MemoryNode*)list, block);
        oldStart = tgt.start;
        tgt.start = (char*)(size + (u32)ptr);
        if ((u32)(tgt.end - tgt.start) < 0x10) {
            tgt.start = tgt.end;
        }
        node->size = tgt.start - ptr;
        if ((u32)(tgt.end - tgt.start) >= 0x10) {
            struct MemoryNode* newNode = (MemoryNode*)CreateNode(&tgt, 0x4652);
            LinkNode((MemoryNode*)list, newNode, prev);
        }
        {
            u16 opt = (u16)(mFlags & 0xff);
            u32 len = (u32)(tgt.start - oldStart);
            if (opt & 1) {
                fill = 0;
                MultiStore_Int(fill, oldStart, len);
            }
        }
    } else {
        tgt2.start = (char*)(size + (u32)ptr);
        tgt2.end = (char*)(node->size + ((u32)node + 0x10));
        node->size = size;
        if (!_ZN22ExpandingHeapAllocator8FreeNodeEP10MemoryNodePNS0_6TargetE(list, &tgt2)) {
            node->size = blockSize;
        }
    }

    return node->size;
}
