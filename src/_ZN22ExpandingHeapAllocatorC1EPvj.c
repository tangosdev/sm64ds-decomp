// NONMATCHING: base materialization / addressing (div=6). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef unsigned int u32;
typedef unsigned short u16;

typedef struct {
    void *start;
    void *end;
} Target;

typedef struct MemoryNode MemoryNode;

extern void _ZN13HeapAllocatorC1EjPvPvj(void *this, u32 magic, void *a, void *b, u32 size);
extern MemoryNode *_ZN22ExpandingHeapAllocator10CreateNodeEPN10MemoryNode6TargetEt(Target *t, u16 v);

void *_ZN22ExpandingHeapAllocatorC1EPvj(char *this, void *start, u32 size) {
    char *inner = this + 0x24;
    Target t;
    MemoryNode *node;
    u16 v;
    _ZN13HeapAllocatorC1EjPvPvj(this, 0x45585048, inner + 0x14, start, size);
    *(u16 *)(inner + 0x10) = 0;
    *(u16 *)(inner + 0x12) = 0;
    v = *(u16 *)(inner + 0x12);
    *(u16 *)(inner + 0x12) = v & ~1;
    t.start = *(void **)(this + 0x18);
    t.end = *(void **)(this + 0x1c);
    node = _ZN22ExpandingHeapAllocator10CreateNodeEPN10MemoryNode6TargetEt(&t, 0x4652);
    *(MemoryNode **)inner = node;
    *(MemoryNode **)(inner + 4) = node;
    *(u32 *)(inner + 8) = 0;
    *(u32 *)(inner + 0xc) = 0;
    return this;
}
