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
 * AllocateNode still carries its raw mangled name: it takes MemoryNode parameters this
 * header cannot spell without reconstructing MemoryNode as a class with its nested
 * Target -- see the commit message.
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
  return _ZN22ExpandingHeapAllocator12AllocateNodeEP10MemoryNodeS1_Pvjj(c, best, bestTarget, size, 1);
}
