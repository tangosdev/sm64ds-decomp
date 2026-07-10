//cpp
/* ExpandingHeap::VSizeof(void*) at 0x0203c444 -- Heap vtable slot.
 * Node headers carry their own size, so this ignores `this` and forwards
 * to the static ExpandingHeapAllocator::SizeofInternal(void*). */

typedef unsigned int u32;

class ExpandingHeapAllocator
{
public:
    static u32 SizeofInternal(void* ptr);
};

class ExpandingHeap
{
public:
    u32 VSizeof(void* ptr);
};

u32 ExpandingHeap::VSizeof(void* ptr)
{
    return ExpandingHeapAllocator::SizeofInternal(ptr);
}
