//cpp
extern "C" {
extern void _ZN18SolidHeapAllocator5ResetEj(void*, unsigned int);
}

struct SolidHeap {
    void VDeallocateAll();
};

void SolidHeap::VDeallocateAll()
{
    char* self = (char*)this;
_ZN18SolidHeapAllocator5ResetEj(*(void**)(self + 0x14), 3);

}
