//cpp
#include "Heap.h"

extern "C" {

extern int func_0203d974(void);
extern void *func_02011e94(int *size);
extern mHeap::Heap_t *data_0209d520;
/* Unless this console is the host (func_0203d974), make data_0209d520 a
   root heap on the block func_02011e94 takes from the sound heap
   sSndHeap, its size in size. Return data_0209d520 either way.
   dScMb_c makes it the default heap while it loads archive 7. */
void *func_0201a3e4(void) {
    int size;
    if (func_0203d974()) {
        data_0209d520 = mHeap::Heap_t::CreateRootHeap(func_02011e94(&size), size);
    }
    return data_0209d520;
}
}
