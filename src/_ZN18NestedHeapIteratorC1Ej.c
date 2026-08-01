#include "types.h"
// @symbol _ZN18NestedHeapIteratorC1Ej
/* recovered: named members + shared header */
#include "NestedHeapIterator.h"
struct Iter {
    void* head;
    void* tail;
    u16 count;
    u16 off;
};

void _ZN18NestedHeapIteratorC1Ej(void* self, u32 off)
{
    struct Iter* it = (struct Iter*)self;
    it->head = 0;
    it->tail = 0;
    it->count = 0;
    it->off = (u16)off;
}
