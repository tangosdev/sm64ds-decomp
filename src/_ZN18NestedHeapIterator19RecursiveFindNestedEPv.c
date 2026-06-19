typedef unsigned int u32;
extern void* _ZN18NestedHeapIterator4NextEP13HeapAllocator(void* self, void* cur);

void* _ZN18NestedHeapIterator19RecursiveFindNestedEPv(void* self, void* addr)
{
    void* cur = _ZN18NestedHeapIterator4NextEP13HeapAllocator(self, 0);
    while (cur != 0) {
        if (*(void**)((char*)cur + 0x18) <= addr && addr < *(void**)((char*)cur + 0x1c)) {
            void* r = _ZN18NestedHeapIterator19RecursiveFindNestedEPv((char*)cur + 0xc, addr);
            if (r == 0) r = cur;
            return r;
        }
        cur = _ZN18NestedHeapIterator4NextEP13HeapAllocator(self, cur);
    }
    return (void*)0;
}
