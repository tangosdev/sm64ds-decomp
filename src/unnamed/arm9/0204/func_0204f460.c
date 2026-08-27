// func_0204f460 - insert node into iter in sorted order, set back-pointer

typedef struct HeapAllocator HeapAllocator;
typedef struct NestedHeapIteratorT NestedHeapIteratorT;

struct NestedHeapIteratorT {
    HeapAllocator* first;
    int unk4;
    unsigned short count;
    unsigned short linkOffset;
};

struct HeapAllocator {
    char pad[0x04];
    NestedHeapIteratorT* owner;  // +0x04 back-pointer to owner iterator
    char pad2[0x35];             // pad to 0x3d
    unsigned char sortKey;       // +0x3d
};

extern HeapAllocator* _ZN18NestedHeapIterator4NextEP13HeapAllocator(NestedHeapIteratorT* iter, HeapAllocator* current);
extern void _ZN18NestedHeapIterator5AddAtEP13HeapAllocatorS1_(NestedHeapIteratorT* iter, HeapAllocator* pos, HeapAllocator* node);

void func_0204f460(NestedHeapIteratorT* self, HeapAllocator* node) {
    HeapAllocator* cur;
    cur = _ZN18NestedHeapIterator4NextEP13HeapAllocator(self, 0);
    if (cur != 0) {
        for (;;) {
            if (node->sortKey < cur->sortKey)
                break;
            cur = _ZN18NestedHeapIterator4NextEP13HeapAllocator(self, cur);
            if (cur == 0)
                break;
        }
    }
    _ZN18NestedHeapIterator5AddAtEP13HeapAllocatorS1_(self, cur, node);
    node->owner = self;
}