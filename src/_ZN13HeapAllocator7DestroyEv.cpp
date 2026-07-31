//cpp
extern "C" {
extern void _ZN13HeapAllocator6RemoveEv(void);
}

struct HeapAllocator {
    void Destroy(void);
};

void HeapAllocator::Destroy(void)
{
_ZN13HeapAllocator6RemoveEv();

}
