//cpp
extern "C" {
extern unsigned int _ZN18SolidHeapAllocator10MemoryLeftEi(void*, int);
}

struct SolidHeap {
    unsigned int VMaxAllocationUnitSize();
};

unsigned int SolidHeap::VMaxAllocationUnitSize()
{
    char* self = (char*)this;
return _ZN18SolidHeapAllocator10MemoryLeftEi(*(void**)(self + 0x14), 4);

}
