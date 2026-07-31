#include "types.h"
struct MemoryNodeTarget {
    void* start;
    void* end;
};

struct MemoryNode {
    char magic[2];
    u16 flags;
    u32 size;
    struct MemoryNode* prev;
    struct MemoryNode* next;
};

extern void _ZN10MemoryNode6TargetC1EP10MemoryNode(struct MemoryNodeTarget* t, struct MemoryNode* node);
extern void _ZN22ExpandingHeapAllocator10UnlinkNodeEP10MemoryNodeS1_(struct MemoryNode* pair, struct MemoryNode* target);
extern int  _ZN22ExpandingHeapAllocator8FreeNodeEP10MemoryNodePNS0_6TargetE(struct MemoryNode* freeNodePair, struct MemoryNodeTarget* target);

int _ZN22ExpandingHeapAllocator10DeallocateEPv(void* self, void* ptr)
{
    struct MemoryNodeTarget target;
    char* inner;
    struct MemoryNode* node;

    node = (struct MemoryNode*)((char*)ptr - 0x10);
    inner = (char*)self + 0x24;
    _ZN10MemoryNode6TargetC1EP10MemoryNode(&target, node);
    _ZN22ExpandingHeapAllocator10UnlinkNodeEP10MemoryNodeS1_((struct MemoryNode*)(inner + 8), node);
    return _ZN22ExpandingHeapAllocator8FreeNodeEP10MemoryNodePNS0_6TargetE((struct MemoryNode*)inner, &target);
}
