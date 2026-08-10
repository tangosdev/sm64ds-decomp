//cpp
// @symbol _ZN18SolidHeapAllocator9SaveStateEj
#include "SolidHeapAllocator.h"

/* SolidHeapAllocator::SaveState(u32 arg) at 0x0204ea98 -- uses `this`.
 *
 * Pushes a restore point onto a chain THAT LIVES INSIDE THE HEAP ITSELF: the 0x10-byte
 * record is allocated out of the very region it records, by the same AllocateForwards
 * everything else uses. So saving state consumes heap, and LoadState reclaims it by
 * rewinding past the record. Returns 0 if the allocation fails, 1 otherwise.
 *
 * Record layout: {id, saved begin, saved end, previous record}. `saved` is read before
 * the allocation, so the restore point is the state BEFORE the record was carved out.
 *
 * `inline_fn` is left exactly as recovered. It is not a helper worth naming -- it is
 * how the original expressed `this + 0x24` in a way that produces the ROM's register
 * allocation, and folding it into the caller changes the bytes.
 *
 * AllocateForwards is a real static member call now, replacing the hand-spelled
 * `extern void *_ZN18SolidHeapAllocator16AllocateForwardsEPvjj(...)` this file used to
 * carry -- which in a C++ TU would have mangled a second time.
 */
struct FreeList
{
    void *begin;
    void *end;
    void *tail;
};

inline struct FreeList *inline_fn(void *arg0)
{
    return (struct FreeList *)(((char *)arg0) + 0x24);
}

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
