// NONMATCHING: base materialization / addressing (div=1). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef unsigned int u32;

struct FreeList { void* begin; void* end; int flags; };

extern void _ZN13HeapAllocatorC1EjPvPvj(void* self, u32 magic, void* a, void* b, u32 size);

void* _ZN18SolidHeapAllocatorC1EPvj(void* c, void* ptr, u32 size)
{
    struct FreeList* fl = (struct FreeList*)((char*)c + 0x24);
    _ZN13HeapAllocatorC1EjPvPvj(c, 0x46524d48, (char*)fl + 0xc, ptr, size);
    fl->begin = *(void**)((char*)c + 0x18);
    fl->end = *(void**)((char*)c + 0x1c);
    fl->flags = 0;
    return c;
}
