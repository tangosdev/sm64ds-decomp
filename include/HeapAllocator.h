/* Reconstructed from HeapAllocator's constructor and iterator methods. */
#ifndef HEAPALLOCATOR_H
#define HEAPALLOCATOR_H
#include "types.h"
#include "NestedHeapIterator.h"

struct HeapAllocator {
    u32 mMagic;                    /* 0x000 */
    HeapAllocator* mPrevious;     /* 0x004 */
    HeapAllocator* mNext;         /* 0x008 */
    NestedHeapIterator mChildren; /* 0x00c */
    void* mStart;                 /* 0x018 */
    void* mEnd;                   /* 0x01c */
    u32 mFlags;                   /* 0x020 */
#ifdef __cplusplus
    HeapAllocator() {}

    void Remove();
    void Destroy();
#endif
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char HeapAllocator_size_must_be_0x24[
    sizeof(struct HeapAllocator) == 0x24 ? 1 : -1];
#endif

#endif
