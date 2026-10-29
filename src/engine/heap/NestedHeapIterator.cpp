//cpp
/* arm9/NestedHeapIterator -- the intrusive doubly-linked list every HeapAllocator
 * carries at +0xc, plus the two functions that drive it: HeapAllocator::Remove
 * and func_0204df54, the shared allocator initializer both heap families call
 * from their constructors.
 * .text 0x0204dcfc..0x0204e084, folded from twelve legacy shards.
 * mwccarm emits .text in reverse source order, so the definitions below run
 * ROM-descending; the roster reads ROM-ascending.
 *
 *   Previous/Next      -- walk the embedded link pair.
 *   Remove             -- unlink one node and fix head/tail.
 *   AddAt/AddFirst/AddLast/Init -- splice in at a position, either end, or an
 *                          empty list.
 *   C1                 -- linkOffset + empty list.
 *   HeapAllocator::Remove -- remove self from whichever iterator owns it.
 *   func_0204df54      -- the shared initializer: stamps magic/range/flags,
 *                          constructs the child iterator, builds the global
 *                          root iterator once, and links self under it.
 *   FindNested         -- the innermost iterator that owns an address (static;
 *                          starts from the file-scope root, not from this).
 *   RecursiveFindNested -- the walk proper.
 *
 * The link pair lives at mLinkOffset bytes into each HeapAllocator -- prev at
 * +0, next at +4 -- so the list walks through char* offsets rather than named
 * members: the offset is a runtime value the layout cannot spell.
 *
 * deslop leftovers:
 * - Init keeps its recovered new_var/double-add scaffolding (the link slot
 *   is formed twice, once into new_var and once inline); the u16 puns on
 *   mCount and mLinkOffset it used to carry measured byte-neutral as plain
 *   member spellings and are gone.
 * - func_0204df54 calls the ctor by its mangled spelling: the explicit
 *   NestedHeapIterator(u32) C1 call on &mChildren is the matched ABI seam --
 *   the body registers child lists on an already-built object, so the
 *   original could not have been an in-place `new'.
 */

#include "HeapAllocator.h"
#include "NestedHeapIterator.h"
#include "decl_NestedHeapIterator.h"

extern int data_020a4d34;
extern "C" NestedHeapIterator data_020a4d38;

// @symbol _ZN18NestedHeapIterator19RecursiveFindNestedEPv
/* The walk: for the first node whose [mStart, mEnd) contains addr, recurse
 * into that node's own child list before falling back to the node itself. */
void *NestedHeapIterator::RecursiveFindNested(void *addr)
{
    HeapAllocator *cur = (HeapAllocator *)Next(0);
    while (cur != 0) {
        if (cur->mStart <= addr && addr < cur->mEnd) {
            void *r = cur->mChildren.RecursiveFindNested(addr);
            if (r == 0) r = cur;
            return r;
        }
        cur = (HeapAllocator *)Next(cur);
    }
    return (void *)0;
}

// @symbol _ZN18NestedHeapIterator10FindNestedEPv
/* The public entry over RecursiveFindNested: start from the root iterator and
 * hand back the innermost iterator that owns `addr', or the root itself. */
NestedHeapIterator *NestedHeapIterator::FindNested(void *addr)
{
    NestedHeapIterator *root = &data_020a4d38;
    HeapAllocator *found = (HeapAllocator *)root->RecursiveFindNested(addr);
    if (found) {
        return &found->mChildren;
    }
    return root;
}

// @symbol func_0204df54
/* Shared allocator initializer -- not a constructor: its final AddLast call
 * leaves the iterator count in r0, where a constructor restores `this'. The
 * callers pass the last argument as u32 while the body reads it as u16. */
extern "C" void func_0204df54(HeapAllocator* self, u32 magic, void* start,
                               void* end, u16 flags)
{
    self->mMagic = magic;
    self->mStart = start;
    self->mEnd = end;
    self->mFlags = 0;
    self->mFlags &= ~0xffu;
    self->mFlags |= flags & 0xffu;

    _ZN18NestedHeapIteratorC1Ej(&self->mChildren, 4);

    if (data_020a4d34 == 0) {
        _ZN18NestedHeapIteratorC1Ej(&data_020a4d38, 4);
        data_020a4d34 = 1;
    }

    NestedHeapIterator::FindNested(self)->AddLast(self);
}

// @symbol _ZN13HeapAllocator6RemoveEv
/* Remove self from whichever iterator owns it. */
void HeapAllocator::Remove()
{
    NestedHeapIterator* iter = NestedHeapIterator::FindNested(this);
    iter->Remove(this);
}

// @symbol _ZN18NestedHeapIteratorC1Ej
NestedHeapIterator::NestedHeapIterator(u32 linkOffset)
    : mFirst(0), mLast(0), mCount(0), mLinkOffset(linkOffset)
{
}

// @symbol _ZN18NestedHeapIterator4InitEP13HeapAllocator
/* Empty list: link the node to itself and make it both ends. */
void NestedHeapIterator::Init(HeapAllocator * a_)
{
    char * a = (char *)a_;

    char *new_var = a + mLinkOffset;
    *((int *)((a + mLinkOffset) + 4)) = 0;
    *((int *)new_var) = 0;
    mFirst = (HeapAllocator *)a;
    mLast = (HeapAllocator *)a;
    mCount += 1;
}

// @symbol _ZN18NestedHeapIterator7AddLastEP13HeapAllocator
void NestedHeapIterator::AddLast(HeapAllocator * a_)
{
    char * a = (char *)a_;

    if (mFirst == 0) {
        Init((HeapAllocator *)a);
        return;
    }
    {
        unsigned short link_off = mLinkOffset;
        int last = (int)mLast;
        *(int *)(a + link_off) = last;
        *(int *)(a + link_off + 4) = 0;
        link_off = mLinkOffset;
        last = (int)mLast;
        *(int *)((char *)last + link_off + 4) = (int)a;
        mLast = (HeapAllocator*)a;
        mCount += 1;
    }
}

// @symbol _ZN18NestedHeapIterator8AddFirstEP13HeapAllocator
void NestedHeapIterator::AddFirst(HeapAllocator * a_)
{
    char* a = (char*)a_;

    if (mFirst == 0) {
        Init((HeapAllocator *)a);
        return;
    }
    {
        int* pa = (int*)(a + mLinkOffset);
        pa[0] = 0;
        pa[1] = (int)mFirst;
    }
    *(int *)((char *)mFirst + mLinkOffset) = (int)a;
    mFirst = (HeapAllocator *)a;
    mCount += 1;
}

// @symbol _ZN18NestedHeapIterator5AddAtEP13HeapAllocatorS1_
/* Splice `node_' in immediately before `at_'. A null `at_' means append and
 * `at_' being the head means prepend, so both ends delegate; only the middle
 * case does the four-pointer relink. */
void NestedHeapIterator::AddAt(HeapAllocator *at_, HeapAllocator *node_)
{
    if (at_ == 0) {
        AddLast(node_);
        return;
    }
    if (at_ == mFirst) {
        AddFirst(node_);
        return;
    }
    {
        unsigned short off = mLinkOffset;
        char *nlink = (char *)node_ + off;
        char *prev = *(char **)((char *)at_ + off);
        *(char **)nlink = prev;
        *(char **)(nlink + 4) = (char *)at_;
        *(char **)(prev + off + 4) = (char *)node_;
        *(char **)((char *)at_ + mLinkOffset) = (char *)node_;
        mCount += 1;
    }
}

// @symbol _ZN18NestedHeapIterator6RemoveEP13HeapAllocator
void NestedHeapIterator::Remove(HeapAllocator * node_)
{
    void* node = (void*)node_;

    u16 off = this->mLinkOffset;
    void* prev = *(void**)((char*)node + off);
    if (prev == 0) {
        this->mFirst = *(HeapAllocator**)((char*)node + off + 4);
    } else {
        *(void**)((char*)prev + off + 4) = *(void**)((char*)node + off + 4);
    }
    void* next = *(void**)((char*)node + off + 4);
    if (next == 0) {
        this->mLast = *(HeapAllocator**)((char*)node + off);
    } else {
        u16 off2 = this->mLinkOffset;
        *(void**)((char*)next + off2) = *(void**)((char*)node + off);
    }
    *(void**)((char*)node + off) = 0;
    *(void**)((char*)node + off + 4) = 0;
    mCount -= 1;
}

// @symbol _ZN18NestedHeapIterator4NextEP13HeapAllocator
int NestedHeapIterator::Next(HeapAllocator * h_)
{
    char* h = (char*)h_;

    if (h == 0)
        return (int)mFirst;
    unsigned short off = mLinkOffset;
    return *(int *)(h + off + 4);
}

// @symbol _ZN18NestedHeapIterator8PreviousEP13HeapAllocator
int NestedHeapIterator::Previous(HeapAllocator * h_)
{
    char* h = (char*)h_;

  if (h == 0) return (int)mLast;
  unsigned short off = mLinkOffset;
  return *(int*)(h+off);
}
