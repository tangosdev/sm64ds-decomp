//cpp
// @symbol _ZN22ExpandingHeapAllocatorC1EPvj
#include "ExpandingHeapAllocator.h"
#include "decl_HeapAllocator.h"

ExpandingHeapAllocator::ExpandingHeapAllocator(void* heapEnd, u32 flags)
{
    char* nodeList = (char*)this + 0x24;
    MemoryNode::Target extent;
    MemoryNode* node;

    func_0204df54(this, 0x45585048, nodeList + 0x14, heapEnd, flags);
    *(u16*)(nodeList + 0x10) = 0;
    *(u16*)(nodeList + 0x12) = 0;
    *(u16*)(nodeList + 0x12) &= ~1;

    extent.start = (char*)mStart;
    extent.end = (char*)mEnd;
    node = (MemoryNode*)CreateNode(&extent, 0x4652);
    *(MemoryNode**)(nodeList + 0x0) = node;
    *(MemoryNode**)(nodeList + 0x4) = node;
    *(u32*)(nodeList + 0x8) = 0;
    *(u32*)(nodeList + 0xc) = 0;
}
