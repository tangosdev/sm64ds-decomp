//cpp
// @symbol _ZN4Heap8_DestroyEv
/* Heap::_Destroy() at 0x0203c74c -- a three-word tail-call veneer to
 * Heap::Destroy (0x0203c758): `ldr ip,[pc] / bx ip / .word 0x0203c758'.
 *
 * PROBE: can a veneer be a real method? It used to be spelled as an
 * argument-less extern "C" function calling another argument-less extern "C"
 * function, both by mangled name -- which reproduces the bytes because a tail
 * call never touches the arguments, and which is also why the shape survived
 * unexamined. Written as a real member forwarding real arguments it should emit
 * the same three words. */
#include "Heap.h"

void Heap::_Destroy()
{
    Destroy();
}
