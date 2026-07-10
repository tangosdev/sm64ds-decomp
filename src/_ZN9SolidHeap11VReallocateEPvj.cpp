//cpp
/* SolidHeap::VReallocate(void*, unsigned) at 0x0203c558 -- Heap vtable slot.
 * Forwards to the allocator (SolidHeapAllocator* at this+0x14). */

typedef unsigned int u32;

class SolidHeapAllocator
{
public:
    void* Reallocate(void* ptr, u32 size);
};

class SolidHeap
{
public:
    u32 unk00;
    u32 unk04;
    u32 unk08;
    u32 unk0c;
    u32 unk10;
    SolidHeapAllocator* allocator; /* 0x14 */

    void* VReallocate(void* ptr, u32 size);
};

void* SolidHeap::VReallocate(void* ptr, u32 size)
{
    return allocator->Reallocate(ptr, size);
}
