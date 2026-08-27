//cpp
// @symbol _ZN4Heap18InitializeGameHeapEjPS_
/* Heap::InitializeGameHeap(u32, Heap*) at 0x0203c24c -- carve the game heap out
 * of `root' and publish it. Alignment 4 is hard-coded here; callers do not get
 * a say. */
#include "Heap.h"

/* 0x020a0eac, already named in config/arm9/symbols.txt. */
extern "C" Heap* GAME_HEAP_PTR;

void Heap::InitializeGameHeap(u32 size, Heap* root)
{
    GAME_HEAP_PTR = CreateExpandingHeap(size, root, 4);
}
