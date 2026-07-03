typedef unsigned int u32;

struct FreeList { void* begin; void* end; int flags; };

void _ZN18SolidHeapAllocator10ResetStartEv(void* c)
{
    void *begin;
    struct FreeList *fl;

    begin = *(void **)((char *)c + 0x18);
    fl = (struct FreeList *)((char *)c + 0x24);
    *(void **)(((long long)(int)fl) & 0xFFFFFFFFFFFFFFFFLL) = begin;
    fl->flags = 0;
}