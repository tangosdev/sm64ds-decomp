// func_0204f504 - dequeue a HeapAllocator from indexed sub-iterator, set cross-links

typedef struct HeapAllocator HeapAllocator;
typedef struct NestedHeapIteratorT NestedHeapIteratorT;

struct NestedHeapIteratorT {
    HeapAllocator* first;
    int unk4;
    unsigned short count;
    unsigned short linkOffset;
};

// Element in the array at 0x020a4d6c, stride 0x1c
typedef struct {
    char pad[0xc];
    NestedHeapIteratorT iter; // +0x0c
    char pad2[4];             // +0x18, total 0x1c
} IndexEntry; // sizeof = 0x1c

struct HeapAllocator {
    int unk0;
    int unk4;
    void* unk8;  // +0x08 (return value / back pointer)
    void* unkc;  // +0x0c (set to r6)
};

extern HeapAllocator* _ZN18NestedHeapIterator4NextEP13HeapAllocator(NestedHeapIteratorT* iter, HeapAllocator* current);
extern void _ZN18NestedHeapIterator6RemoveEP13HeapAllocator(NestedHeapIteratorT* iter, HeapAllocator* node);

// Array at 0x020a4d6c
extern IndexEntry data_020a4d6c[];

void* func_0204f504(int index, void* unk) {
    IndexEntry* entry = &data_020a4d6c[index];
    HeapAllocator* first = _ZN18NestedHeapIterator4NextEP13HeapAllocator(&entry->iter, 0);
    if (first == 0)
        return 0;
    _ZN18NestedHeapIterator6RemoveEP13HeapAllocator(&entry->iter, first);
    first->unkc = unk;
    ((HeapAllocator**)((char*)unk))[2] = first; /* unk[8] = first */
    return first->unk8;
}