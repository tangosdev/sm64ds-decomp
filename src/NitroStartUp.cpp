//cpp
#include "types.h"
#include "Heap.h"
#include "nitro/os/init.h"

extern "C" {

extern void func_0201a490(void);
extern void func_02019ebc(void);
}

extern mHeap::Heap_t *data_020a0e9c;

/* NitroSDK's startup hook. crt0 calls it after _fp_init and before
 * __call_static_initializers (_start, src/_start.c). The SDK's own copy is
 * empty, and a game supplies its own. This one runs OS_Init, then the
 * hook func_0201a490 and the card pull-out callback (func_02019ebc), and
 * builds the root heap, so the heap exists before any static constructor
 * runs. */
void NitroStartUp(void) {
    OS_Init();
    func_0201a490();
    func_02019ebc();
    mHeap::Heap_t::InitializeRootHeap();
    data_020a0e9c->SetNodeID(1);
}
