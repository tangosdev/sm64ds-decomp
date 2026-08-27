// func_0204f400 - insert HeapAllocator into sorted global list (by sortKey field)

typedef struct HeapAllocator HeapAllocator;
typedef struct NestedHeapIteratorT NestedHeapIteratorT;

struct NestedHeapIteratorT {
    HeapAllocator* first;
    int unk4;
    unsigned short count;
    unsigned short linkOffset;
};

struct HeapAllocator {
    char pad[0x3d];
    unsigned char sortKey; // +0x3d
};

extern HeapAllocator* _ZN18NestedHeapIterator4NextEP13HeapAllocator(NestedHeapIteratorT* iter, HeapAllocator* current);
extern void _ZN18NestedHeapIterator5AddAtEP13HeapAllocatorS1_(NestedHeapIteratorT* iter, HeapAllocator* pos, HeapAllocator* node);

// The global NestedHeapIterator struct at 0x020a4d60
extern NestedHeapIteratorT data_020a4d60;

void func_0204f400(HeapAllocator* self) {
    HeapAllocator* cur;
    cur = _ZN18NestedHeapIterator4NextEP13HeapAllocator(&data_020a4d60, 0);
    if (cur != 0) {
        for (;;) {
            if (self->sortKey < cur->sortKey)
                break;
            cur = _ZN18NestedHeapIterator4NextEP13HeapAllocator(&data_020a4d60, cur);
            if (cur == 0)
                break;
        }
    }
    _ZN18NestedHeapIterator5AddAtEP13HeapAllocatorS1_(&data_020a4d60, cur, self);
}