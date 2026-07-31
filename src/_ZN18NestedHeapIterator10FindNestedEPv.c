#include "types.h"
// NestedHeapIterator::FindNested(void* ptr) - static
// Address: 0x0204dfe8
// Finds the innermost NestedHeapIterator containing ptr; returns rootHeapIterator if none found
struct NestedHeapIterator;
struct HeapAllocator;

struct NestedHeapIterator {
    struct HeapAllocator* first; // 0x00
    struct HeapAllocator* last;  // 0x04
    u16 nestedHeaps;             // 0x08
    u16 nestedIterOffset;        // 0x0a
};

struct HeapAllocator {
    char magic[4];                          // 0x00
    struct HeapAllocator* prev;             // 0x04
    struct HeapAllocator* next;             // 0x08
    struct NestedHeapIterator nestedHeapIt; // 0x0c
};

extern struct HeapAllocator* _ZN18NestedHeapIterator19RecursiveFindNestedEPv(
    struct NestedHeapIterator* self, void* ptr);
extern struct NestedHeapIterator _ZN6Memory16rootHeapIteratorE;

struct NestedHeapIterator* _ZN18NestedHeapIterator10FindNestedEPv(void* ptr) {
    struct NestedHeapIterator* root = &_ZN6Memory16rootHeapIteratorE;
    struct HeapAllocator* found = _ZN18NestedHeapIterator19RecursiveFindNestedEPv(root, ptr);
    if (found) {
        return &found->nestedHeapIt;
    }
    return root;
}