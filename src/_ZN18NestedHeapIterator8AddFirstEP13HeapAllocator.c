#define AT(p, off) ((void *)(int)(((long long)(int)((char *)(p) + (off))) & 0xFFFFFFFFFFFFFFFFLL))
struct HeapAllocator;
extern void _ZN18NestedHeapIterator4InitEP13HeapAllocator(char* it, char* a);
void _ZN18NestedHeapIterator8AddFirstEP13HeapAllocator(char* it, char* a) {
    if (*(char**)it == 0) { _ZN18NestedHeapIterator4InitEP13HeapAllocator(it, a); return; }
    {
        int* pa = (int*)(a + *(unsigned short*)(it + 0xa));
        pa[0] = 0;
        pa[1] = *(int*)it;
    }
    *(int*)(*(char**)it + *(unsigned short*)(it + 0xa)) = (int)a;
    *(char**)it = a;
    {
        unsigned short* cnt = (unsigned short*)AT(it, 8);
        *cnt = *cnt + 1;
    }
}
