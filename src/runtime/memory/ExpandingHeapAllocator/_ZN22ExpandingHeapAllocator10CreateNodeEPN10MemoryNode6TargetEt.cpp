//cpp
// @symbol _ZN22ExpandingHeapAllocator10CreateNodeEPN10MemoryNode6TargetEt
#include "ExpandingHeapAllocator.h"

/* ExpandingHeapAllocator::CreateNode(MemoryNode::Target*, u16 tag) at 0x0204e8b0 --
 * STATIC. Two declared parameters (`PN10MemoryNode6TargetE` + `t`) against two body
 * arguments, so there is no room for a `this`.
 *
 * Stamps a fresh header at the start of an extent and returns it. The size it records
 * is `end - (node + 0x10)` -- user bytes measured from just past the header, not the
 * extent's own length -- which is the definition MemoryNode::Target's constructor
 * inverts to recover the end again. The flags word is zeroed here, so the alignment
 * padding it carries in bits 8..14 is written by whoever placed the node, not by this.
 *
 * THE BODY IS VERBATIM apart from the receiver. The extent arrives as `MemoryNode::
 * Target*` because that is what the ROM's mangled name declares, and is immediately
 * cast back to the `int*` the recovered body indexes -- `c[0]` is start, `c[1]` is end.
 * Rewriting those as `extent->start` / `extent->end` is a body change, and this class
 * has already produced one three-word divergence from exactly that kind of tidying
 * (SetNodeID, #1221). The declaration follows the ROM; the body is left alone.
 */
void* ExpandingHeapAllocator::CreateNode(MemoryNode::Target* extent, u16 t)
{
  int* c = (int*)extent;
  char* n = (char*)c[0];
  *(unsigned short*)(n+0) = t;
  *(unsigned short*)(n+2) = 0;
  *(int*)(n+4) = ((int)c[1]) - (int)(n + 0x10);
  *(int*)(n+8) = 0;
  *(int*)(n+0xc) = 0;
  return n;
}
