// @symbol _ZN18NestedHeapIterator5AddAtEP13HeapAllocatorS1_
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_NestedHeapIterator.h"
/* recovered: named members + shared header */
#include "NestedHeapIterator.h"
extern void _ZN18NestedHeapIterator7AddLastEP13HeapAllocator(char *it, char *a);

void _ZN18NestedHeapIterator5AddAtEP13HeapAllocatorS1_(struct NestedHeapIterator *self, char *at, char *node) {
    if (at == 0) {
        _ZN18NestedHeapIterator7AddLastEP13HeapAllocator(((char *)self), node);
        return;
    }
    if (at == *(char**)((char *)self)) {
        _ZN18NestedHeapIterator8AddFirstEP13HeapAllocator(((char *)self), node);
        return;
    }
    {
        unsigned short off = self->mLinkOffset;
        /* demand node+off first then prev? */
        char *nlink = node + off;
        char *prev = *(char **)(at + off);
        *(char **)nlink = prev;
        *(char **)(nlink + 4) = at;
        *(char **)(prev + off + 4) = node;
        *(char **)(at + self->mLinkOffset) = node;
        *(unsigned short *)(int)((char *)&self->unk_008) += 1;
    }
}
