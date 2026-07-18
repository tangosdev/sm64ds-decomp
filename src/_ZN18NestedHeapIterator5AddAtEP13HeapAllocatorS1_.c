extern void _ZN18NestedHeapIterator7AddLastEP13HeapAllocator(char *it, char *a);
extern void _ZN18NestedHeapIterator8AddFirstEP13HeapAllocator(char *it, char *a);

void _ZN18NestedHeapIterator5AddAtEP13HeapAllocatorS1_(char *it, char *at, char *node)
{
    if (at == 0) {
        _ZN18NestedHeapIterator7AddLastEP13HeapAllocator(it, node);
        return;
    }
    if (at == *(char**)it) {
        _ZN18NestedHeapIterator8AddFirstEP13HeapAllocator(it, node);
        return;
    }
    {
        unsigned short off = *(unsigned short *)(it + 0xa);
        /* demand node+off first then prev? */
        char *nlink = node + off;
        char *prev = *(char **)(at + off);
        *(char **)nlink = prev;
        *(char **)(nlink + 4) = at;
        *(char **)(prev + off + 4) = node;
        *(char **)(at + *(unsigned short *)(it + 0xa)) = node;
        *(unsigned short *)(int)(((long long)(int)(it + 8)) & 0xFFFFFFFFFFFFFFFFLL) += 1;
    }
}
