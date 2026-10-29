//cpp
#include "types.h"
#include "Heap.h"

extern "C" {

/* func_02018908 at 0x02018908 */
extern void func_0204ee10(void *self);

void func_02018908(void *self, void *heap)
{
    func_0204ee10(self);
    ((mHeap::Heap_t *)heap)->_Deallocate(self);
}
}
