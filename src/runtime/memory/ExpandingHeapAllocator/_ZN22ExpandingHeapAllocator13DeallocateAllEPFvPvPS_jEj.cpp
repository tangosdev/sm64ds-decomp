//cpp
// @symbol _ZN22ExpandingHeapAllocator13DeallocateAllEPFvPvPS_jEj
#include "ExpandingHeapAllocator.h"

/* ExpandingHeapAllocator::DeallocateAll(DeallocationFunction, u32) at 0x0204e08c --
 * uses `this`. Two declared parameters against three body arguments, so the leading
 * one is the receiver.
 *
 * Walks the in-use list at `this + 0x2c` and hands each block's user pointer
 * (node + 0x10) to the callback, along with the allocator and the caller's opaque
 * argument. `next` is read BEFORE the call, because the callback frees the node it is
 * given and the link would be gone by the time the loop needed it.
 *
 * THE CALLBACK IS A FUNCTION POINTER, NOT A POINTER TO ONE. The imported symbol said
 * `PPFvPvPS_jE` -- pointer to pointer to function -- which is where the reference
 * header's `DeallocationFunction* func` came from. The ROM refutes it directly:
 *
 *     +0x10   mov r6, r1      the incoming parameter, parked in a callee-saved reg
 *     +0x38   blx r6          called; r6 is never loaded from in between
 *
 * A pointer-to-pointer would need an `ldr r6, [r6]` ahead of that `blx`, and there is
 * no such instruction anywhere in the function. This is positive evidence, not merely
 * the absence of a contradiction. Declared as a plain `DeallocationFunction` the member
 * mangles `PFvPvPS_jEj` and matches at 2004/b56, all 21 words, no relocations involved.
 *
 * #1221 deferred this one as "a real discrepancy worth a look" -- the mangled name and
 * the reproducing body disagreeing about a parameter. Same root cause as AllocateNode's
 * `Pvjj`, and the same source: a name imported from a header written to CALL into the
 * ROM, where nothing about the callee's actual parameter is observable.
 *
 * Note InvokeDeallocate above is exactly this callback's shape -- (void*,
 * ExpandingHeapAllocator*, u32) -- which is what it is for: the trampoline that turns
 * each block back into an allocator->Deallocate(ptr) call.
 */
void* ExpandingHeapAllocator::DeallocateAll(DeallocationFunction fn, u32 ctx) {
  void* node = *(void**)((char*)this + 0x2c);
  if (!node) return node;
  do {
    void* next = *(void**)((char*)node + 0xc);
    fn((char*)node + 0x10, this, ctx);
    node = next;
  } while (node);
  return node;
}
