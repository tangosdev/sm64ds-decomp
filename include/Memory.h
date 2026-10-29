#ifndef MEMORY_H
#define MEMORY_H
#include "types.h"

namespace mHeap { struct Heap_t; }
typedef mHeap::Heap_t Heap;  /* as in include/Heap.h */

namespace Memory {

extern Heap *defaultHeapPtr;

void* operator_new2(u32 size);
void  operator_delete2(void* ptr);

/* src/engine/heap/Heap.cpp. A null heap means the current one; the short
   forms pass 4 for the alignment and null for the heap. Unnamed: a named
   parameter takes an @NNN in every includer. */
void *Allocate(u32, int, Heap *);
void *Allocate(u32, int);
void *Allocate(u32);
void Deallocate(void *, Heap *);
void Deallocate(void *);
}

#endif
