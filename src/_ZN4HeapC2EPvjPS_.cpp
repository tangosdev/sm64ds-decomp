//cpp
// @symbol _ZN4HeapC2EPvjPS_
#include "Heap.h"

Heap::Heap(void* start, u32 size, Heap* root)
    : heapStart(start), heapSize(size), parentHeap(root), flags(0)
{
    flags = 0x4000;
}
