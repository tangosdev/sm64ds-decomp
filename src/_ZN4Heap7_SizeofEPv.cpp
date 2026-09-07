//cpp
// @symbol _ZN4Heap7_SizeofEPv
/* Heap::_Sizeof(void*) at 0x0203c274 -- three-word tail-call veneer to
 * Heap::Sizeof (0x0203c454): `ldr ip,[pc] / bx ip / .word 0x0203c454'. */
#include "Heap.h"

int Heap::_Sizeof(void* ptr)
{
    return Sizeof(ptr);
}
