//cpp
extern "C" {
int func_0204f0b8(unsigned int bit);
void _ZN18NestedHeapIterator6RemoveEP13HeapAllocator(void* self, void* h);
}

extern int data_020a552c[];

extern "C" void func_0204fda4(char* c)
{
    int* p;
    func_0204f0b8(*(unsigned int*)(c + 0x28));
    _ZN18NestedHeapIterator6RemoveEP13HeapAllocator(data_020a552c, c);
    p = (int*)((((long long)(int)(c + 0xc)) & 0xFFFFFFFFFFFFFFFFLL));
    *p &= ~1;
    *p &= ~2;
}
