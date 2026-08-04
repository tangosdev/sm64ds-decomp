//cpp
#include "types.h"
// @symbol _ZN13HeapAllocatorC1EjPvPvj
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_NestedHeapIterator.h"
#include "decl_common.h"
/* recovered: named members + shared header */
#include "HeapAllocator.h"
extern int data_020a4d34;
extern char data_020a4d38;
extern "C" {
extern void* _ZN18NestedHeapIterator10FindNestedEPv(void* ptr);
extern void _ZN18NestedHeapIterator7AddLastEP13HeapAllocator(void* iter, void* node);


void _ZN13HeapAllocatorC1EjPvPvj(struct HeapAllocator *self, u32 magic, void* a, void* b, unsigned short size) {
    *(u32*)((char*)self) = magic;
    *(void**)((char*)&self->unk_018) = a;
    *(void**)((char*)&self->unk_01c) = b;
    self->unk_020 = 0;
    {
        u32* p = (u32*)(((long long)(int)((char*)&self->unk_020)));
        *p &= ~0xffu;
        *p |= (size & 0xffu);
    }
    _ZN18NestedHeapIteratorC1Ej(((char*)self) + 0xc, 4);
    if (data_020a4d34 == 0) {
        _ZN18NestedHeapIteratorC1Ej(&data_020a4d38, 4);
        data_020a4d34 = 1;
    }
    {
        void* it = _ZN18NestedHeapIterator10FindNestedEPv(((char*)self));
        _ZN18NestedHeapIterator7AddLastEP13HeapAllocator(it, ((char*)self));
    }
}

}
