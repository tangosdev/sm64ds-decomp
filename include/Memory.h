#ifndef MEMORY_H
#define MEMORY_H
#ifndef __cplusplus
#define Deallocate _Z10DeallocatePv
#endif
#include "types.h"

namespace mHeap { struct Heap_t; }
typedef mHeap::Heap_t Heap;  /* as in include/Heap.h */

namespace Memory {

extern Heap *defaultHeapPtr;

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
