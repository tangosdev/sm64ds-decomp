extern void _ZN18NestedHeapIterator4InitEP13HeapAllocator(char *it, char *a);

void _ZN18NestedHeapIterator7AddLastEP13HeapAllocator(char *it, char *a)
{
    if (*(int *)it == 0) { _ZN18NestedHeapIterator4InitEP13HeapAllocator(it, a); return; }
    {
        unsigned short link_off = *(unsigned short *)(it + 0xa);
        int last = *(int *)(it + 4);
        *(int *)(a + link_off) = last;
        *(int *)(a + link_off + 4) = 0;
        link_off = *(unsigned short *)(it + 0xa);
        last = *(int *)(it + 4);
        *(int *)((char *)last + link_off + 4) = (int)a;
        *(int *)(it + 4) = (int)a;
        *(unsigned short *)(int)(((long long)(int)(it + 8)) & 0xFFFFFFFFFFFFFFFFLL) += 1;
    }
}
