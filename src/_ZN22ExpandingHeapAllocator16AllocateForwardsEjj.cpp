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
 * AllocateNode still carries its raw mangled name: it takes MemoryNode parameters this
 * header cannot spell without reconstructing MemoryNode as a class with its nested
 * Target -- see the commit message. Migration is per-reference.
 */
extern "C" {

struct MemoryNode {
  unsigned short tag;
  unsigned short flags;
  unsigned int size;
  MemoryNode* prev;
  MemoryNode* next;
};

struct NodeList {
  MemoryNode* head;
  MemoryNode* tail;
  unsigned char pad[0xa];
  unsigned short flag;
};

void* _ZN22ExpandingHeapAllocator12AllocateNodeEP10MemoryNodeS1_Pvjj(NodeList* c, MemoryNode* node, void* target, unsigned int size, unsigned int z);

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
  return _ZN22ExpandingHeapAllocator12AllocateNodeEP10MemoryNodeS1_Pvjj(c, best, bestTarget, size, 0);
}
