//cpp
/* SolidHeap::VAllocate(unsigned, unsigned) at 0x0203c6ac -- Heap vtable slot.
 * Forwards to the allocator (SolidHeapAllocator* at this+0x14). */

typedef unsigned int u32;

class SolidHeapAllocator
{
public:
    void* Allocate(u32 size, int align);
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

    void* VAllocate(u32 size, u32 align);
};

void* SolidHeap::VAllocate(u32 size, u32 align)
{
    return allocator->Allocate(size, align);
}
