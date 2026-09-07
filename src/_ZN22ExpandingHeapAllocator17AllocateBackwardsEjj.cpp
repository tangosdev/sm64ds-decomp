//cpp
// @symbol _ZN22ExpandingHeapAllocator17AllocateBackwardsEjj
#include "ExpandingHeapAllocator.h"

/* ExpandingHeapAllocator::AllocateBackwards(u32 size, u32 align) at 0x0204e504 --
 * uses `this`. `Ejj` is two declared parameters, three body arguments, so the leading
 * one is `this`.
 *
 * The mirror of AllocateForwards: walks the free list from the TAIL and aligns
 * DOWNWARD -- `(node->size + data - size) & ~mask` puts the block at the high end of
 * the node, so the alignment padding lands below it rather than above. The fit test is
 * therefore `aligned - data >= 0` (did it stay inside the node) rather than a
 * size-plus-padding comparison.
 *
 * Same first-fit/best-fit flag as the forward search, and the same trailing 1 vs 0 to
 * AllocateNode telling it which end was taken.
 *
 * AllocateNode is now a declared member and is called as one, so the call target is
 * mangled from the declaration rather than hand-spelled here. Its fifth parameter is a
 * u16 (`Pvjt`, not the imported `Pvjj`); the flag is stack-passed and AAPCS widens it to
 * a word regardless, so these bytes are unchanged by that correction.
 */
extern "C" {

/* MemoryNode now comes from include/MemoryNode.h via ExpandingHeapAllocator.h.
   The local copy this file used to carry was identical to it. */

struct NodeList {
  MemoryNode* head;
  MemoryNode* tail;
  unsigned char pad[0xa];
  unsigned short flag;
};

}

void* ExpandingHeapAllocator::AllocateBackwards(u32 size, u32 align)
{
  NodeList* c = (NodeList*)((char*)this + 0x24);
  unsigned short flag = c->flag;
  int firstFit = ((unsigned short)(flag & 1) == 0);
  MemoryNode* best = 0;
  MemoryNode* node = c->tail;
  unsigned int bestSize = 0xFFFFFFFF;
  void* bestTarget = 0;
  if (node != 0) {
    unsigned int mask = align - 1;
    do {
      char* data = (char*)node + 0x10;
      unsigned int nsize = node->size;
      unsigned int t = nsize + (unsigned int)data - size;
      char* aligned = (char*)(~mask & t);
      if ((int)(aligned - data) >= 0 && bestSize > nsize) {
        best = node;
        bestSize = nsize;
        bestTarget = aligned;
        if (firstFit) break;
        if (nsize == size) break;
      }
      node = node->prev;
    } while (node != 0);
  }
  if (best == 0) return 0;
  return AllocateNode((MemoryNode*)c, best, bestTarget, size, 1);
}
