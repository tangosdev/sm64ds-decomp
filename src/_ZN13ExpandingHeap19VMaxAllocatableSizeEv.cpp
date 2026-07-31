//cpp
extern "C" {
extern unsigned int _ZN22ExpandingHeapAllocator18MaxAllocatableSizeEi(void*, int);
}

struct ExpandingHeap {
    unsigned int VMaxAllocatableSize();
};

unsigned int ExpandingHeap::VMaxAllocatableSize()
{
    char* self = (char*)this;
return _ZN22ExpandingHeapAllocator18MaxAllocatableSizeEi(*(void**)(self + 0x14), 4);

}
