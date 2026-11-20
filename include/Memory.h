#ifndef MEMORY_H
#define MEMORY_H
#include "types.h"

namespace mHeap { struct Heap_t; }
typedef mHeap::Heap_t Heap;  /* as in include/Heap.h */

namespace Memory {

extern Heap *defaultHeapPtr;
/* The root heap, built by Heap_t::SetupRootHeap. */
extern Heap *rootHeapPtr;
/* The default heap that SetupSolidHeapAsDefault set aside, until
   RestoreFromTemporary reinstates it. */
extern Heap *tmpHeapPtr;
/* Set by Heap_t::Intact the first time any heap fails its check, and never
   cleared: the first corrupted heap in the run is the one worth reporting. */
extern u8 heapDamaged;

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
