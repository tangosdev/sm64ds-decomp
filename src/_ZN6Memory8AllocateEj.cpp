//cpp
#include "types.h"
class Heap;

namespace Memory
{
    void* Allocate(u32 size, int align, Heap* heap);

    void* Allocate(u32 size)
    {
        return Allocate(size, 4, 0);
    }
}
