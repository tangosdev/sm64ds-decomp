//cpp
#include "types.h"
#include "Heap.h"

extern "C" {

/* func_02018908 at 0x02018908 */
extern int func_0204ee10(void *archive);

void func_02018908(void *archive, void *heap) {
    func_0204ee10(archive);
    ((mHeap::Heap_t *)heap)->_Deallocate(archive);
}
}
