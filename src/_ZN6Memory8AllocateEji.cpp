//cpp
/* Memory::Allocate(unsigned, int) at 0x0203c1d8 -- convenience overload that
 * forwards to Memory::Allocate(unsigned, int, Heap*) with a null heap. */

typedef unsigned int u32;

class Heap;

namespace Memory
{
    void* Allocate(u32 size, int align, Heap* heap);

    void* Allocate(u32 size, int align)
    {
        return Allocate(size, align, 0);
    }
}
