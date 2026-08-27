#include "types.h"
/* func_02018908 at 0x02018908 */
extern void func_0204ee10(void *self);
extern void _ZN4Heap11_DeallocateEPv(void *heap, void *ptr);

void func_02018908(void *self, void *heap)
{
    func_0204ee10(self);
    _ZN4Heap11_DeallocateEPv(heap, self);
}
