//cpp
/* arm9/SolidHeapAllocator -- the solid-heap allocator TU: the allocator that
 * carves one free region from both ends, the Heap factory that embeds the
 * allocator inside the block it manages, and the saved-state chain kept in
 * the free region's own third word.
 * .text 0x0204e964..0x0204eda4, folded from fourteen legacy shards.
 * mwccarm emits .text in reverse source order, so the definitions below run
 * ROM-descending (C1 first, Reallocate last); the roster reads ROM-ascending.
 *
 *   Reallocate         -- in-place resize of the most recent forward alloc.
 *   TryResizeToFit     -- shrink the heap to its used extent.
 *   LoadState/SaveState -- restore points on a chain that lives inside the
 *                          heap it records: the 0x10-byte record is allocated
 *                          out of the very region it captures, and LoadState
 *                          reclaims it by rewinding past it.
 *   MemoryLeft         -- bytes a forward allocation could still get.
 *   Reset/ResetStart/ResetEnd -- rewind begin, end, or both.
 *   Allocate           -- the sign of `align` picks the direction.
 *   func_0204ebb8      -- tail-call veneer to HeapAllocator::Remove.
 *   Heap::CreateSolidHeapAllocator -- fit the allocator into the block.
 *   AllocateForwards/AllocateBackwards -- the static region carvers; they
 *                          take the free region, not the allocator, which is
 *                          what makes them static (see the header).
 *   C1                 -- the constructor; seals the free region.
 *
 * The free region is `mFreeRegion` (this + 0x24). Two views of it live here:
 * the header's SolidHeapFreeRegion {begin, end, flags}, and the TU-local
 * FreeList {begin, end, tail} for the state-chain head the flags word
 * carries at runtime.
 *
 * deslop leftovers:
 * - MemoryLeft keeps its recovered register-pressure scaffolding verbatim
 *   (new_var*, the empty `if (!(a - 1u)) {}`, the (long long)(int)
 *   round-trips): each is load-bearing under -O4,p; tidying any changes the
 *   bytes.
 * - SaveState keeps inline_fn, the recorded load-bearing spelling of
 *   `this + 0x24` in that body.
 * - Reallocate keeps the hoisted `fl` pair pointer and TryResizeToFit the
 *   opaque `struct H*` view: spelling the fields as members changes the
 *   register allocation and kills the aliased re-read of mEnd the ROM emits.
 */

#include "Heap.h"
#include "SolidHeapAllocator.h"
#include "decl_HeapAllocator.h"
#include "cstd.h"

extern "C" void MultiStore_Int(s32 val, int *dst, s32 count);
/* local extern: the factory runs the constructor over an already-aligned
   address; a placement new would add a null test the ROM does not make. */
extern "C" SolidHeapAllocator* _ZN18SolidHeapAllocatorC1EPvj(
    SolidHeapAllocator* self, void* heapEnd, u32 flags);

/* The chain view of mFreeRegion: `tail` is the most recent SaveState record. */
struct FreeList {
    void *begin;
    void *end;
    void *tail;
};

/* A SaveState record, allocated inside the heap it describes:
   {id, saved begin, saved end, previous record}. */
struct State {
    u32 id;
    void *head;
    void *tail;
    struct State *prev;
};

/* TryResizeToFit's opaque pair view of mFreeRegion's first two words: kept
   because member spelling lets the compiler prove mEnd cannot alias and it
   drops the re-read the ROM emits. */
struct H {
    void *begin;
    void *end;
};

inline struct FreeList *inline_fn(void *arg0)
{
    return (struct FreeList *)&((SolidHeapAllocator *)arg0)->mFreeRegion;
}

// @symbol _ZN18SolidHeapAllocatorC1EPvj
SolidHeapAllocator::SolidHeapAllocator(void* heapEnd, u32 flags)
{
    SolidHeapFreeRegion* freeRegion = &mFreeRegion;

    func_0204df54(this, 0x46524d48, (char*)freeRegion + 0xc,
                  heapEnd, flags);
    freeRegion->begin = mStart;
    freeRegion->end = mEnd;
    freeRegion->flags = 0;
}
// @symbol _ZN18SolidHeapAllocator16AllocateForwardsEPvjj
/* Carves `size` bytes off the low end: round begin up to `align`, refuse if
 * that runs past end, then push begin past the block. `align` is already a
 * positive power of two here; Allocate took the absolute value and picked the
 * direction. */
void *SolidHeapAllocator::AllocateForwards(void *freeBlockPair, u32 size, u32 align)
{
    void **pair = (void **)freeBlockPair;
    void *freeBlockBegin = pair[0];
    void *start = (void *)(((align - 1) + (u32)freeBlockBegin) & ~(align - 1));
    void *freeBlockEnd = pair[1];
    void *end = (void *)((u32)size + (u32)start);
    if (end > freeBlockEnd) {
        return (void *)0;
    }
    u32 flags = *(u32 *)((char *)freeBlockPair - 4);
    u32 fillSize = (u32)end - (u32)freeBlockBegin;
    if ((u16)(flags & 0xff) & 1) {
        volatile s32 zero = 0;
        MultiStore_Int(zero, (int *)freeBlockBegin, fillSize);
    }
    pair[0] = end;
    return start;
}

// @symbol _ZN18SolidHeapAllocator17AllocateBackwardsEPvjj
/* Carves `size` bytes off the HIGH end: subtract from end, round the result
 * DOWN to `align`, refuse if that runs below begin, then pull end down.
 * Because the rounding is downward the alignment gap lands above the block
 * rather than below it, so the returned pointer is also the new end -- which
 * is why `start` is both stored and returned, where the forward case stores
 * `end` and returns `start`.
 *
 * The flags word sits 4 bytes BEFORE the pair -- mFlags of the allocator.
 * Bit 0 means fill-on-allocate, and the fill covers begin..end, including the
 * alignment gap, not just the returned block. */
void *SolidHeapAllocator::AllocateBackwards(void *freeBlockPair, u32 size, u32 align)
{
    void **pair = (void **)freeBlockPair;
    void *freeBlockEnd = pair[1];
    void *freeBlockBegin = pair[0];
    void *start = (void *)(((u32)freeBlockEnd - size) & ~(align - 1));
    if (start < freeBlockBegin) {
        return (void *)0;
    }
    u32 flags = *(u32 *)((char *)freeBlockPair - 4);
    u32 fillSize = (u32)freeBlockEnd - (u32)start;
    if ((u16)(flags & 0xff) & 1) {
        volatile s32 zero = 0;
        MultiStore_Int(zero, (int *)start, fillSize);
    }
    pair[1] = start;
    return start;
}

// @symbol _ZN18SolidHeapAllocator10ResetStartEv
/* Rewinds the free region's begin pointer back to the heap's start and clears
 * its flags, discarding every forward allocation at once. ResetEnd is the
 * mirror. */
void SolidHeapAllocator::ResetStart()
{
    void *begin;
    SolidHeapFreeRegion *fl;

    begin = mStart;
    fl = &mFreeRegion;
    *(void **)(fl) = begin;
    fl->flags = 0;
}

// @symbol _ZN18SolidHeapAllocator8ResetEndEv
/* Walks the saved-state chain and rewinds every entry's end pointer to the
 * heap's hard end, then rewinds the live free region's end too. LoadState can
 * then restore any of them without them still pointing into space that has
 * been reclaimed. */
void SolidHeapAllocator::ResetEnd()
{
    int *node, *p = (int *)&mFreeRegion;
    for (node = (int *)p[2]; node; node = (int *)node[3])
        node[2] = (int)mEnd;
    p[1] = (int)mEnd;
}

// @symbol _ZN5mHeap6Heap_t24CreateSolidHeapAllocatorEPvjj
/* Fit a SolidHeapAllocator into [address, address + size), or refuse. Both ends
 * are brought to a 4-byte boundary, inwards, so the aligned span is never
 * larger than asked for. The refusal has two halves: `start > end` catches
 * rounding crossing the ends on a tiny or misaligned block, and 0x30 is the
 * allocator's own bookkeeping, below which there would be nothing left to
 * hand out.
 *
 * The allocator is constructed AT start_u: the object lives inside the block
 * it manages. That is why the constructor takes the end as its argument and
 * needs no separate size. */
namespace mHeap {

SolidHeapAllocator* Heap_t::CreateSolidHeapAllocator(void* address, u32 size, u32 flags)
{
    u32 end   = (size + (u32)address) & ~3u;
    u32 start = ((u32)address + 3u)   & ~3u;

    if (start > end || (end - start) < 0x30)
        return 0;

    return _ZN18SolidHeapAllocatorC1EPvj((SolidHeapAllocator*)start, (void*)end, flags);
}

} /* namespace mHeap */

// @symbol func_0204ebb8
/* Tail-call veneer to HeapAllocator::Remove -- a three-word thunk
 * (ldr ip,[pc] / bx ip / .word), the same shape Heap.h's _-prefixed veneers
 * take. */
extern "C" void func_0204ebb8(SolidHeapAllocator *allocator)
{
    allocator->Remove();
}

// @symbol _ZN18SolidHeapAllocator8AllocateEji
/* ALIGN'S SIGN IS THE DIRECTION, not part of the alignment: non-negative
 * allocates from the low end of the free region, negative from the high end.
 * A zero size is rounded up to 1 so every allocation has a distinct address,
 * then the size is rounded up to a multiple of 4.
 *
 * The helpers take the free region rather than `this`, which is what makes
 * them static -- see the header. */
void *SolidHeapAllocator::Allocate(u32 size, int align)
{
    void *fb = &mFreeRegion;
    if (size == 0) size = 1;
    size = (size + 3) & ~3;
    if (align >= 0) {
        return AllocateForwards(fb, size, (u32)align);
    }
    return AllocateBackwards(fb, size, (u32)(-align));
}

// @symbol _ZN18SolidHeapAllocator5ResetEj
/* bit 0: rewind the free region's begin; bit 1: rewind its end. Passing 3
 * empties the heap in both directions; passing 0 is a no-op the ROM still
 * emits. */
void SolidHeapAllocator::Reset(u32 params)
{
    if (params & 1) {
        ResetStart();
    }
    if (params & 2) {
        ResetEnd();
    }
}

// @symbol _ZN18SolidHeapAllocator10MemoryLeftEi
/* How many bytes a forward allocation at this alignment could still get: round
 * the free region's begin up to `align`, subtract from its end, and clamp at 0.
 * `align` goes through cstd::abs, so the direction convention Allocate uses
 * (negative means backwards) is accepted here and ignored.
 *
 * THE SCAFFOLDING IS LOAD-BEARING AND STAYS VERBATIM. `new_var`/`new_var2`/
 * `new_var3`, the empty `if (!(a - 1u)) {}`, the re-read of begin in the return
 * expression instead of reusing `aligned`, and the `(long long)(int)`
 * round-trips are all register-pressure steering: they are how this recovered
 * source reproduces the ROM's scheduling under -O4,p. Tidying any of them
 * changes the bytes. That includes reading begin as `mFreeRegion.begin`,
 * plainly or through the launder as `&mFreeRegion.begin`: both change it. */
int SolidHeapAllocator::MemoryLeft(int align)
{
    u32 a;
    char *new_var2;
    u32 mask;
    u32 new_var;
    int **new_var3;
    int *fb;
    u32 aligned;
    u32 end;

    a = (u32)cstd::abs(align);
    mask = a - 1u;
    fb = (int *)&mFreeRegion;
    aligned = (mask + (u32)*(int *)((long long)(int)((char *)this + 0x24))) & ~(a - 1u);
    end = (u32)fb[1];
    if (!(a - 1u)) {
    }
    new_var = end;
    if (aligned > new_var) {
        return 0;
    }
    new_var2 = (char *)this;
    new_var3 = &fb;
    fb = *new_var3;
    return (int)(end - ((mask + (u32)*(int *)((long long)(int)(new_var2 + 0x24))) & ~(a - 1u)));
}

// @symbol _ZN18SolidHeapAllocator9SaveStateEj
/* Pushes a restore point onto the chain: `saved` is read before the
 * allocation, so the restore point is the state BEFORE the record was carved
 * out. Returns 0 if the allocation fails, 1 otherwise. */
int SolidHeapAllocator::SaveState(u32 arg)
{
    struct FreeList *fb;
    void *saved;
    int *p;

    fb = inline_fn(this);
    saved = *(void **)(fb);
    p = (int *)AllocateForwards(fb, 0x10, 4);
    if (!p)
        return 0;
    p[0] = (int)arg;
    p[1] = (int)saved;
    p[2] = (int)fb->end;
    p[3] = (int)fb->tail;
    fb->tail = p;
    return 1;
}

// @symbol _ZN18SolidHeapAllocator9LoadStateEj
/* Pops back to a restore point pushed by SaveState. `id == 0` means "the most
 * recent one" and skips the search entirely -- note the search loop is inside
 * `if (id != 0)`, so a zero id never compares against a record's id even if
 * some record carries 0. Returns 0 when the chain is empty or no record
 * matches, 1 on success.
 *
 * Restoring rewinds begin, end and the chain head together, which reclaims the
 * record itself along with everything allocated after it -- the records live
 * in the heap they describe (see SaveState). */
s32 SolidHeapAllocator::LoadState(u32 id)
{
    struct State *st;
    struct FreeList *fb = (struct FreeList *)&mFreeRegion;
    st = (struct State *)fb->tail;
    if (id != 0) {
        while (st != 0) {
            if (st->id == id) break;
            st = st->prev;
        }
    }
    if (st == 0) return 0;
    fb->begin = st->head;
    fb->end = st->tail;
    fb->tail = st->prev;
    return 1;
}

// @symbol _ZN18SolidHeapAllocator14TryResizeToFitEv
/* Shrinks the heap to exactly what has been allocated, but only when nothing
 * has been allocated backwards: the guard compares the hard end (mEnd) against
 * the free region's end and bails returning 0 if they differ. On success the
 * hard end is pulled down to the free region's begin and the used size is
 * returned -- a byte count measured from the object itself. */
s32 SolidHeapAllocator::TryResizeToFit()
{
    struct H *base = (struct H *)&mFreeRegion;
    void *cur = mEnd;
    if ((s32)((char *)cur - (char *)base->end) != 0) return 0;
    mEnd = base->begin;
    base->end = mEnd;
    return (s32)((char *)mEnd - (char *)this);
}

// @symbol _ZN18SolidHeapAllocator10ReallocateEPvj
/* Resizes IN PLACE, and only for the most recent forward allocation: `curSize`
 * is measured as `freeRegion.begin - ptr`, so the block being resized is assumed
 * to end exactly where the free region starts. Returns the new size, or 0 if
 * growing would overrun the free region's end.
 *
 * Shrinking cannot fail and skips the fill entirely -- the guard is
 * `size > curSize`, so the flags word only matters when growing, and then only
 * the newly exposed bytes are cleared. */
u32 SolidHeapAllocator::Reallocate(void *ptr, u32 size)
{
    void **fl = (void **)&mFreeRegion;
    if (size == 0) size = 1;
    size = (size + 3) & ~3;
    void *begin = fl[0];
    u32 newEnd = size + (u32)ptr;
    u32 curSize = (u32)begin - (u32)ptr;
    if (size == curSize) {
        return size;
    }
    if (size > curSize) {
        if ((s32)(newEnd - (u32)fl[1]) > 0) {
            return 0;
        }
        u32 flags = mFlags;
        if ((u16)(flags & 0xff) & 1) {
            volatile s32 zero = 0;
            MultiStore_Int(zero, (int *)begin, size - curSize);
        }
    }
    fl[0] = (void *)newEnd;
    return size;
}

