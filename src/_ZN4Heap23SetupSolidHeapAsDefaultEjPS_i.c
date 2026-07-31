// @symbol _ZN4Heap23SetupSolidHeapAsDefaultEjPS_i
/* recovered: named members + shared header */
#include "Heap.h"
// Heap::SetupSolidHeapAsDefault: creates a SolidHeap, saves default to tmp, sets new heap as default

typedef void Heap;

extern Heap* _ZN6Memory14defaultHeapPtrE;
extern Heap* _ZN6Memory10tmpHeapPtrE;

extern Heap* _ZN4Heap15CreateSolidHeapEjPS_i(unsigned int size, Heap* root, int align);
extern Heap* _ZN4Heap10SetDefaultEv(Heap* self);

void* _ZN4Heap23SetupSolidHeapAsDefaultEjPS_i(unsigned int size, Heap* root, int align) {
    Heap* heap = _ZN4Heap15CreateSolidHeapEjPS_i(size, root, align);
    if (!heap) return (void*)0;
    _ZN6Memory10tmpHeapPtrE = _ZN6Memory14defaultHeapPtrE;
    _ZN4Heap10SetDefaultEv(heap);
    return heap;
}
