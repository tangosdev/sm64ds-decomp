//cpp
// @symbol _ZN6Memory8AllocateEjiP4Heap
//
// Language-mode flip: Memory::Allocate(u32, int, Heap*) — compiler mangles the
// name. Body shape preserved (null heap falls back to defaultHeapPtr, then
// Heap::Allocate). Sibling overloads already migrated the same way.
#include "types.h"

class Heap
{
public:
    void* Allocate(u32 size, int align);
};

namespace Memory
{
    extern Heap* defaultHeapPtr; /* 0x020a0ea0 / _ZN6Memory14defaultHeapPtrE */

    void* Allocate(u32 size, int align, Heap* heap)
    {
        if (!heap)
            heap = defaultHeapPtr;
        return heap->Allocate(size, align);
    }
}
