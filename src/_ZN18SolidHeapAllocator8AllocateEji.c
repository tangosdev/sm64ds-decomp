typedef unsigned int u32;

extern void* _ZN18SolidHeapAllocator16AllocateForwardsEPvjj(void* pair, u32 size, u32 align);
extern void* _ZN18SolidHeapAllocator17AllocateBackwardsEPvjj(void* pair, u32 size, u32 align);

void* _ZN18SolidHeapAllocator8AllocateEji(void* c, u32 size, int align)
{
    void* fb = (char*)c + 0x24;
    if (size == 0) size = 1;
    size = (size + 3) & ~3;
    if (align >= 0) {
        return _ZN18SolidHeapAllocator16AllocateForwardsEPvjj(fb, size, (u32)align);
    }
    return _ZN18SolidHeapAllocator17AllocateBackwardsEPvjj(fb, size, (u32)(-align));
}
