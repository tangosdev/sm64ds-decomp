//cpp
#include "types.h"
// @symbol _ZN18NestedHeapIterator6RemoveEP13HeapAllocator
/* recovered: named members + shared header, real C++ method */
#include "NestedHeapIterator.h"
struct HeapAllocator;
struct NHI { void* head; void* tail; u16 count; u16 off; };

void NestedHeapIterator::Remove(HeapAllocator * node_)
{
    void* node = (void*)node_;

    u16 off = ((struct NHI*)this)->off;
    void* prev = *(void**)((char*)node + off);
    if (prev == 0) {
        ((struct NHI*)this)->head = *(void**)((char*)node + off + 4);
    } else {
        *(void**)((char*)prev + off + 4) = *(void**)((char*)node + off + 4);
    }
    void* next = *(void**)((char*)node + off + 4);
    if (next == 0) {
        ((struct NHI*)this)->tail = *(void**)((char*)node + off);
    } else {
        u16 off2 = ((struct NHI*)this)->off;
        *(void**)((char*)next + off2) = *(void**)((char*)node + off);
    }
    *(void**)((char*)node + off) = 0;
    *(void**)((char*)node + off + 4) = 0;
    *(u16 *)(((u32)((struct NHI*)this) + 8) & 0xFFFFFFFFFFFFFFFFULL) -= 1;
}
