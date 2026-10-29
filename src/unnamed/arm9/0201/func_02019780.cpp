//cpp
#include "types.h"
#include "Heap.h"

extern "C" {

/* func_02019780 at 0x02019780 */
extern void func_02058c84(void);
extern void func_0201a490(void);
extern void func_02019ebc(void);
extern mHeap::Heap_t *data_020a0e9c;

void func_02019780(void) {
    func_02058c84();
    func_0201a490();
    func_02019ebc();
    mHeap::Heap_t::InitializeRootHeap();
    data_020a0e9c->SetNodeID(1);
}
}
