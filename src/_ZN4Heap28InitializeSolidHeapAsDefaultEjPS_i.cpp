//cpp
// @symbol _ZN4Heap28InitializeSolidHeapAsDefaultEjPS_i
/* Heap::InitializeSolidHeapAsDefault(u32, Heap*, int) at 0x0203c2d8 --
 * three-word tail-call veneer to Heap::SetupSolidHeapAsDefault (0x0203c2e4):
 * `ldr ip,[pc] / bx ip / .word 0x0203c2e4'.
 *
 * Static, by the same argument-count test as its target: three declared
 * parameters, three arguments in the body, no room for a `this'. Note the
 * veneer sits twelve bytes BEFORE what it jumps to -- the pair is adjacent and
 * the thunk is still a long-form `ldr/bx', which is what these veneers look
 * like throughout: they are not distance-driven. */
#include "Heap.h"

void* Heap::InitializeSolidHeapAsDefault(u32 size, Heap* root, int align)
{
    return SetupSolidHeapAsDefault(size, root, align);
}
