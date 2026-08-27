//cpp
// @symbol _ZN4Heap11_DeallocateEPv
/* Heap::_Deallocate(void*) at 0x0203c280 -- three-word tail-call veneer to
 * Heap::Deallocate (0x0203c538): `ldr ip,[pc] / bx ip / .word 0x0203c538'. */
#include "Heap.h"

void Heap::_Deallocate(void* ptr)
{
    Deallocate(ptr);
}
