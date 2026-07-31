//cpp
extern "C" {
extern unsigned int _ZN18SolidHeapAllocator10MemoryLeftEi(void*, int);
}

struct SolidHeap {
    unsigned int VMemoryLeft();
};

unsigned int SolidHeap::VMemoryLeft()
{
    char* self = (char*)this;
return _ZN18SolidHeapAllocator10MemoryLeftEi(*(void**)(self + 0x14), 4);

}
