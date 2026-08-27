//cpp
// @symbol _ZN4Heap9_AllocateEji
/* Heap::_Allocate(u32, int) at 0x0203c29c -- three-word tail-call veneer to
 * Heap::Allocate(u32, int) (0x0203c6cc):
 * `ldr ip,[pc] / bx ip / .word 0x0203c6cc'.
 *
 * The forwarded arguments are real now. The old spelling declared both this
 * function and its target as taking NOTHING -- `void f(void)' calling
 * `void g(void)' -- which reproduces the bytes precisely because a tail call
 * leaves r0-r3 untouched. That is the veneer's whole nature, and it is also
 * what let the wrong prototypes sit here unchallenged. */
#include "Heap.h"

void* Heap::_Allocate(u32 size, int align)
{
    return Allocate(size, align);
}
