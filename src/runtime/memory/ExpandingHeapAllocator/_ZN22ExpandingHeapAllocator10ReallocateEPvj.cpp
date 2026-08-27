//cpp
#include "types.h"
/* ExpandingHeapAllocator::Reallocate(void* ptr, u32 size) at 0x0204e1e8
 * Grows a block in place by swallowing the free node that directly follows it,
 * or shrinks it and hands the tail back to the free list.
 *
 * The user pointer sits 0x10 bytes past its MemoryNode header; the allocator's
 * free-node list head lives at this+0x24, and this+0x20 holds the flag word whose
 * low bit asks for freshly-gained bytes to be zero filled. */
extern "C" {

struct MemoryNode {
    char magic[2];
    u16 flags;
    u32 size;
    struct MemoryNode* prev;
    struct MemoryNode* next;
};

struct Target {
    char* start;
    char* end;
};

void _ZN10MemoryNode6TargetC1EP10MemoryNode(struct Target* t, struct MemoryNode* node);
struct MemoryNode* _ZN22ExpandingHeapAllocator10UnlinkNodeEP10MemoryNodeS1_(void* list, struct MemoryNode* node);
struct MemoryNode* _ZN22ExpandingHeapAllocator10CreateNodeEPN10MemoryNode6TargetEt(struct Target* t, u16 tag);
void _ZN22ExpandingHeapAllocator8LinkNodeEP10MemoryNodeS1_S1_(void* list, struct MemoryNode* node, struct MemoryNode* prev);
void MultiStore_Int(int val, void* dst, int len);
int _ZN22ExpandingHeapAllocator8FreeNodeEP10MemoryNodePNS0_6TargetE(void* list, struct Target* t);

static inline struct MemoryNode** GetList(void* t) { return (struct MemoryNode**)((char*)t + 0x24); }

u32 _ZN22ExpandingHeapAllocator10ReallocateEPvj(void* thiz, char* ptr, u32 size)
{
    struct Target tgt;
    struct Target tgt2;
    volatile int fill;
    struct MemoryNode* block;
    u32 blockSize;
    struct MemoryNode* node;
    struct MemoryNode** list;

    node = (struct MemoryNode*)ptr;
    node = (struct MemoryNode*)((char*)node - 0x10);
    list = GetList(thiz);
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

        _ZN10MemoryNode6TargetC1EP10MemoryNode(&tgt, block);
        prev = _ZN22ExpandingHeapAllocator10UnlinkNodeEP10MemoryNodeS1_(list, block);
        oldStart = tgt.start;
        tgt.start = (char*)(size + (u32)ptr);
        if ((u32)(tgt.end - tgt.start) < 0x10) {
            tgt.start = tgt.end;
        }
        node->size = tgt.start - ptr;
        if ((u32)(tgt.end - tgt.start) >= 0x10) {
            struct MemoryNode* newNode = _ZN22ExpandingHeapAllocator10CreateNodeEPN10MemoryNode6TargetEt(&tgt, 0x4652);
            _ZN22ExpandingHeapAllocator8LinkNodeEP10MemoryNodeS1_S1_(list, newNode, prev);
        }
        {
            u16 opt = (u16)(*(u32*)((char*)thiz + 0x20) & 0xff);
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

}
