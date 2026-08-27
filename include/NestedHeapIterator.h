/* Reconstructed from the iterator methods and constructor. */
#ifndef NESTEDHEAPITERATOR_H
#define NESTEDHEAPITERATOR_H
#include "types.h"

/* fwd */
struct HeapAllocator;
struct NestedHeapIterator {
    HeapAllocator* mFirst;      /* 0x000 */
    HeapAllocator* mLast;       /* 0x004 */
    u16 mCount;                 /* 0x008 */
    u16 mLinkOffset;            /* 0x00a */
#ifdef __cplusplus
    NestedHeapIterator() {}
    NestedHeapIterator(u32 linkOffset);

    int Next(HeapAllocator * h_);
    int Previous(HeapAllocator * h_);
    void AddAt(HeapAllocator * at_, HeapAllocator * node_);
    void AddFirst(HeapAllocator * a_);
    void AddLast(HeapAllocator * a_);
    /* STATIC: the matched body at 0x0204dfe8 takes the pointer in r0 and reads
       no second register, and it starts from the file-scope root iterator rather
       than from an object. Itanium never encodes `this', so the mangled name
       cannot settle this either way -- the argument count does. */
    static NestedHeapIterator * FindNested(void * addr);
    void Init(HeapAllocator * a_);
    void Remove(HeapAllocator * node_);
    void* RecursiveFindNested(void * addr);
#endif
};

typedef char NestedHeapIterator_size_must_be_0xc[
    sizeof(struct NestedHeapIterator) == 0xc ? 1 : -1];

#endif
