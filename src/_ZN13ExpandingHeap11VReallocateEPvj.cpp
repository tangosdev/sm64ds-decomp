//cpp
/* ExpandingHeap::VReallocate(void*, unsigned) at 0x0203c568 -- Heap vtable slot.
 * Forwards to the allocator (ExpandingHeapAllocator* at this+0x14). */

typedef unsigned int u32;

class ExpandingHeapAllocator
{
public:
    void* Reallocate(void* ptr, u32 size);
};

class ExpandingHeap
{
public:
    u32 unk00;
    u32 unk04;
    u32 unk08;
    u32 unk0c;
    u32 unk10;
    ExpandingHeapAllocator* allocator; /* 0x14 */

    void* VReallocate(void* ptr, u32 size);
};

void* ExpandingHeap::VReallocate(void* ptr, u32 size)
{
    return allocator->Reallocate(ptr, size);
}
