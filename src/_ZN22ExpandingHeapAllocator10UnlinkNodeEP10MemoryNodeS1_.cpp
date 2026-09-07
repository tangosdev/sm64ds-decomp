//cpp
// @symbol _ZN22ExpandingHeapAllocator10UnlinkNodeEP10MemoryNodeS1_
#include "ExpandingHeapAllocator.h"

/* ExpandingHeapAllocator::UnlinkNode(MemoryNode* list, MemoryNode* node) at 0x0204e910
 * -- STATIC. Two declared parameters, two body arguments, no room for a `this`.
 *
 * Ordinary doubly-linked-list removal, with the list head and tail written directly
 * when the node was at either end: c[0] is the head, c[1] the tail. Returns the node's
 * former predecessor, which is what FreeNode needs in order to decide whether the freed
 * block can merge backwards.
 *
 * `list` is typed MemoryNode* because that is what the ROM's mangled name declares, but
 * callers pass the allocator's embedded node-list at `this + 0x24`. The body's local
 * `N` and `int*` views are left exactly as recovered -- only the receiver types moved.
 */
struct N { int p0,p1,prev,next; };

void* ExpandingHeapAllocator::UnlinkNode(MemoryNode* list, MemoryNode* node_)
{
  int* c = (int*)list;
  N* node = (N*)node_;
  N* r2 = (N*)node->prev;
  N* r1 = (N*)node->next;
  if (r2) r2->next = (int)r1;
  else c[0] = (int)r1;
  if (r1) r1->prev = (int)r2;
  else c[1] = (int)r2;
  return r2;
}
