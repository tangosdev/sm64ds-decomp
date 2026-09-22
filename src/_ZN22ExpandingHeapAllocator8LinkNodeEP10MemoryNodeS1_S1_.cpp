//cpp
// @symbol _ZN22ExpandingHeapAllocator8LinkNodeEP10MemoryNodeS1_S1_
#include "ExpandingHeapAllocator.h"

/* ExpandingHeapAllocator::LinkNode(MemoryNode* list, MemoryNode* node,
 * MemoryNode* prev) at 0x0204e8e0 -- STATIC. Three declared parameters, three body
 * arguments, no room for a `this`.
 *
 * Inserts `node` immediately after `prev`, or at the head when `prev` is null, fixing
 * both neighbours and the list's head/tail (c[0], c[1]). The inverse of UnlinkNode, and
 * the reason UnlinkNode bothers to return the predecessor: FreeNode unlinks a node,
 * merges, and links the result back at the same position.
 *
 * `list` is typed MemoryNode* because the ROM's mangled name says so; callers pass the
 * allocator's node-list at `this + 0x24`. `node` and `prev` are handled as raw ints in
 * the recovered body -- left exactly that way, since every field access here is an
 * explicit offset and retyping them would change the arithmetic mwccarm emits.
 */
void* ExpandingHeapAllocator::LinkNode(MemoryNode* list, MemoryNode* node_, MemoryNode* prev)
{
  int* c = (int*)list;
  int node = (int)node_;
  int r2 = (int)prev;
  char* n = (char*)node;
  *(int*)(n+8) = r2;
  int r3;
  if (r2) { r3 = *(int*)((char*)r2+0xc); *(int*)((char*)r2+0xc) = node; }
  else { r3 = c[0]; c[0] = node; }
  *(int*)(n+0xc) = r3;
  if (r3) { *(int*)((char*)r3+8) = node; }
  else { c[1] = node; }
  return n;
}
