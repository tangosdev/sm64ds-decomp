//cpp
// @symbol _ZN18NestedHeapIterator19RecursiveFindNestedEPv
/* NestedHeapIterator::RecursiveFindNested -- walk this list and, for the first
 * node whose [mStart, mEnd) range contains addr, recurse into that node's
 * own nested list before falling back to the node itself.
 *
 * The recursion target is `(char *)cur + 0xc', a NestedHeapIterator embedded in
 * HeapAllocator's pad_000[0x18]. Nothing matched has pinned that sub-object from
 * the allocator side, so include/HeapAllocator.h does not type it and the cast
 * stays raw. The two range words ARE typed there, and are void* not s32.
 *
 * Next() is declared `int' in include/NestedHeapIterator.h; the ROM returns the
 * node pointer. A return type is not part of an Itanium mangled name, so the
 * call emits the same symbol either way -- cast at the call site rather than
 * edited in the header, which nine other matched functions also read.
 */
#include "NestedHeapIterator.h"
#include "HeapAllocator.h"

void *NestedHeapIterator::RecursiveFindNested(void *addr)
{
    HeapAllocator *cur = (HeapAllocator *)Next(0);
    while (cur != 0) {
        if (cur->mStart <= addr && addr < cur->mEnd) {
            void *r = ((NestedHeapIterator *)((char *)cur + 0xc))->RecursiveFindNested(addr);
            if (r == 0) r = cur;
            return r;
        }
        cur = (HeapAllocator *)Next(cur);
    }
    return (void *)0;
}
