#ifndef MEMORY_H
#define MEMORY_H
#include "types.h"

namespace mHeap { struct Heap_t; }
typedef mHeap::Heap_t Heap;  /* as in include/Heap.h */

namespace Memory {

extern Heap *defaultHeapPtr;

void* operator_new2(u32 size);
void  operator_delete2(void* ptr);

}

#endif
