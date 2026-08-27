#include "types.h"
typedef struct HeapAllocator HeapAllocator;
typedef struct HeapObj HeapObj;
struct HeapObj {
    HeapAllocator* unk0;
    HeapAllocator* heap;
    HeapAllocator* unk8;
};

extern HeapAllocator data_020a4d60;
extern HeapAllocator data_020a4d54;

extern void _ZN18NestedHeapIterator6RemoveEP13HeapAllocator(HeapAllocator* list, HeapAllocator* item);
extern void _ZN18NestedHeapIterator7AddLastEP13HeapAllocator(HeapAllocator* list, HeapAllocator* item);

void func_0204f2d4(HeapObj* obj) {
    HeapAllocator* p0 = obj->unk0;
    if (p0 != 0) {
        *(HeapAllocator**)p0 = 0;
        obj->unk0 = 0;
    }
    HeapAllocator* heap = obj->heap;
    _ZN18NestedHeapIterator6RemoveEP13HeapAllocator(heap, (HeapAllocator*)obj);
    obj->heap = 0;
    if (obj->unk8 != 0) {
        _ZN18NestedHeapIterator7AddLastEP13HeapAllocator((HeapAllocator*)((u8*)heap + 0xc), obj->unk8);
        *(HeapAllocator**)((u8*)obj->unk8 + 0xc) = 0;
        obj->unk8 = 0;
    }
    _ZN18NestedHeapIterator6RemoveEP13HeapAllocator(&data_020a4d60, (HeapAllocator*)obj);
    _ZN18NestedHeapIterator7AddLastEP13HeapAllocator(&data_020a4d54, (HeapAllocator*)obj);
    *(u8*)((u8*)obj + 0x2c) = 0;
}
