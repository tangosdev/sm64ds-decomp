//cpp
/* Memory::Deallocate(void*) at 0x0203c1b4 -- convenience overload that
 * forwards to Memory::Deallocate(void*, Heap*) with a null heap. */

class Heap;

namespace Memory
{
    void Deallocate(void* ptr, Heap* heap);

    void Deallocate(void* ptr)
    {
        Deallocate(ptr, 0);
    }
}
