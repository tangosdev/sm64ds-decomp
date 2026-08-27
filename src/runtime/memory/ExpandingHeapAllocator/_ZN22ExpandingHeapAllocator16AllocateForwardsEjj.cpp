//cpp
// @symbol _ZN22ExpandingHeapAllocator16AllocateForwardsEjj
#include "ExpandingHeapAllocator.h"

/* ExpandingHeapAllocator::AllocateForwards(u32 size, u32 align) at 0x0204e5c8 --
 * uses `this`.
 *
 * `Ejj` is two declared parameters against three body arguments, so the leading one is
 * `this`. Worth contrasting with SolidHeapAllocator's same-named method, which mangles
 * `EPvjj` and IS static: the name is no guide, only the arity is.
 *
 * Walks the free list forward from the head, aligning each node's data start upward.
 * Bit 0 of the list's flag word selects the search: clear means FIRST fit (stop at the
 * first node that fits), set means BEST fit (keep scanning for the smallest). The
 * `nsize == size` break is an exact-fit shortcut that applies to both.
 *
 * AllocateNode is now a declared member and is called as one, so the call target is
 * mangled from the declaration rather than hand-spelled here. That matters for this
 * call in particular: the symbol it used to name ended `Pvjj`, and the fifth parameter
 * is really a u16 (`Pvjt`). The direction flag below is passed on the stack and AAPCS
 * widens it to a word either way, which is exactly why the caller could never have
 * revealed the error -- and why these bytes are unchanged by the correction.
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

void* ExpandingHeapAllocator::AllocateForwards(u32 size, u32 align)
{
  NodeList* c = (NodeList*)((char*)this + 0x24);
  unsigned short flag = c->flag;
  int firstFit = ((unsigned short)(flag & 1) == 0);
  MemoryNode* best = 0;
  MemoryNode* node = c->head;
  unsigned int bestSize = 0xFFFFFFFF;
  void* bestTarget = 0;
  if (node != 0) {
    unsigned int mask = align - 1;
    do {
      char* data = (char*)node + 0x10;
      char* aligned = (char*)((mask + (unsigned int)data) & ~mask);
      unsigned int pad = (unsigned int)(aligned - data);
      unsigned int nsize = node->size;
      if (nsize >= size + pad && bestSize > nsize) {
        best = node;
        bestSize = nsize;
        bestTarget = aligned;
        if (firstFit) break;
        if (nsize == size) break;
      }
      node = node->next;
    } while (node != 0);
  }
  if (best == 0) return 0;
  return AllocateNode((MemoryNode*)c, best, bestTarget, size, 0);
}
