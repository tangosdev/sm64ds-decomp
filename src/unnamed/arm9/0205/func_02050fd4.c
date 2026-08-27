#include "types.h"
struct SolidHeapAllocator;
struct HeapAllocator;

struct NestedHeapIterator {
    char pad[0x10]; /* opaque */
};

struct SolidHeapAllocator *_ZN18SolidHeapAllocator8AllocateEji(struct SolidHeapAllocator *self, unsigned int size, unsigned int align);
void func_02051064(struct HeapAllocator *alloc);
void _ZN18NestedHeapIterator7AddLastEP13HeapAllocator(struct NestedHeapIterator *iter, struct HeapAllocator *alloc);

struct Obj {
    struct SolidHeapAllocator *heapAlloc;
    struct NestedHeapIterator iter;
};

s32 func_02050fd4(struct Obj *self) {
    struct HeapAllocator *r4 = (struct HeapAllocator *)_ZN18SolidHeapAllocator8AllocateEji(self->heapAlloc, 0x14, 4);
    if (!r4) {
        return 0;
    }
    func_02051064(r4);
    _ZN18NestedHeapIterator7AddLastEP13HeapAllocator(&self->iter, r4);
    return 1;
}
