//cpp
/* Memory::Deallocate(void*, Heap*) at 0x0203c1e8 -- falls back to the
 * default heap when no heap is given, then forwards to Heap::Deallocate. */

class Heap
{
public:
    void Deallocate(void* ptr);
};

namespace Memory
{
    extern Heap* defaultHeapPtr;

    void Deallocate(void* ptr, Heap* heap)
    {
        if (!heap)
            heap = defaultHeapPtr;
        heap->Deallocate(ptr);
    }
}
