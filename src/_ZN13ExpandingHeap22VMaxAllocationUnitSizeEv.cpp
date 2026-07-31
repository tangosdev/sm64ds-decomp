//cpp
extern "C" {
extern unsigned int _ZN22ExpandingHeapAllocator18MaxAllocatableSizeEi(void*, int);
}

struct ExpandingHeap {
    unsigned int VMaxAllocationUnitSize();
};

unsigned int ExpandingHeap::VMaxAllocationUnitSize()
{
    char* self = (char*)this;
return _ZN22ExpandingHeapAllocator18MaxAllocatableSizeEi(*(void**)(self + 0x14), 4);

}
