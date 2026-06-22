// NONMATCHING: base materialization / addressing (div=1). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef unsigned int u32;

struct FreeList { void* begin; void* end; int flags; };

void _ZN18SolidHeapAllocator10ResetStartEv(void* c)
{
    struct FreeList* fl = (struct FreeList*)((char*)c + 0x24);
    fl->begin = *(void**)((char*)c + 0x18);
    fl->flags = 0;
}
