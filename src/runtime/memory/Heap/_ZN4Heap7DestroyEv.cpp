//cpp
// @symbol _ZN4Heap7DestroyEv
/* Heap::Destroy() at 0x0203c758 -- tears the heap down: let the concrete heap
 * release its own bookkeeping (slot 2), forget the arena, then hand the block
 * back to the heap this one was carved out of.
 *
 * Was a cast of `this' to a local `struct Base' for the slot-2 call, plus
 * address-cast writes through `&unk_004' / `&unk_008' / `&unk_00c' -- the
 * casts existed because the header declared those fields but nothing would
 * admit what they were. They are named now, so the writes are plain. */
#include "Heap.h"

void Heap::Destroy()
{
    VDestroy();

    heapStart = 0;
    heapSize = 0;

    /* A root heap has no parent to return the block to. */
    Heap* parent = parentHeap;
    if (parent == 0)
        return;

    parent->Deallocate(this);
    parentHeap = 0;
}
