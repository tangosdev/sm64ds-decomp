/* Hand-edited, against evidence. This file used to carry the
 * "AUTO-GENERATED ... by tools/gen_header.py" banner, which was never true --
 * see notes/runbook-type-reconstruction.md section 2.
 *
 * class ExpandingHeap: 10 matched functions, 1 own field, 16 vtable slots.
 *
 * THE ROM'S NAME FOR THIS CLASS IS `mHeap::ExpHeap_t' (RTTI record at
 * arm9:0x02099cec, mangled N5mHeap9ExpHeap_tE). The tree calls it ExpandingHeap
 * and every symbol is spelled that way; see include/Heap.h for why renaming is
 * not done piecemeal.
 *
 * ==== IT DERIVES FROM Heap, AND IT DID NOT SAY SO ====
 *
 * The same defect include/SolidHeap.h carried, and the same evidence settles it:
 *
 *   - The RTTI record at 0x02099cec is kind `si' (single inheritance) with its
 *     base pointing at 0x02099ce4, which is Heap.
 *   - The vtable at 0x02099dd8 is a full sixteen slots shadowing Heap's sixteen
 *     one for one: D1, D0, then the fourteen V* in Heap's slot order. Heap's own
 *     2..15 are null, so Heap is abstract and these are its implementations.
 *   - The constructor _ZN13ExpandingHeapC1EPvjP4HeapP22ExpandingHeapAllocator
 *     calls Heap's constructor first and only then writes 0x14.
 *
 * ==== WHAT AN EXPANDING HEAP IS, NEXT TO A SOLID ONE ====
 *
 * The pair exists because the two allocators trade opposite things. SolidHeap is
 * linear: allocation is a pointer bump, individual blocks can never be freed
 * (VDeallocate crashes), and the three "how much is left" slots all give the
 * same answer because there is only one free region. ExpandingHeap keeps a node
 * list: blocks carry headers, so VSizeof can answer, VDeallocate works, and
 * slots 10/11/12 genuinely differ -- MemoryLeft sums every free node while
 * MaxAllocatableSize reports the largest single one.
 *
 * They also invert on resizing. SolidHeap::VResizeToFit hands its unused tail
 * back to the parent; ExpandingHeap::VResizeToFit returns 0 unconditionally,
 * because a heap whose free space is scattered across a node list has no tail to
 * give.
 *
 * The generator's `fwd' block is dropped -- ExpandingHeap.h's held no stray
 * parameter-name structs, but SolidHeap.h's did, from the same generator.
 */
#ifndef EXPANDINGHEAP_H
#define EXPANDINGHEAP_H
#include "types.h"
#include "Heap.h"

/* ExpandingHeapAllocator is only ever pointed at from here, so a declaration is
 * enough -- no definition is pulled in. The typedef keeps the member spelled the
 * same in C and in C++; the guard is common.h's idiom for the same job. */
#ifndef EXPANDINGHEAPALLOCATOR_FWD_DECLARED
#define EXPANDINGHEAPALLOCATOR_FWD_DECLARED
struct ExpandingHeapAllocator;
typedef struct ExpandingHeapAllocator ExpandingHeapAllocator;
#endif

#ifndef __cplusplus
/* Flat, because C has neither bases nor an implicit vptr. The first five
   members are Heap's, repeated in its order; see include/Heap.h. */
struct ExpandingHeap {
    void* vtable;                       /* 0x00 */
    void* heapStart;                    /* 0x04 */
    u32   heapSize;                     /* 0x08 */
    struct Heap* parentHeap;            /* 0x0c */
    u32   flags;                        /* 0x10 */
    ExpandingHeapAllocator* allocator;  /* 0x14 */
};
#else
struct ExpandingHeap : Heap {
    /* The one field of its own. Everything below 0x14 belongs to the base. */
    ExpandingHeapAllocator* allocator;  /* 0x14 -- node-list allocator; it lives
                                           inside the arena it manages, which is
                                           why Heap::CreateExpandingHeapAllocator
                                           constructs it at the arena's start. */

    /* The constructor's initializer list is the ROM body: construct Heap, then
       store the allocator pointer. No explicit vtable write belongs in source;
       CodeWarrior installs this class's vptr between those two steps. */
    ExpandingHeap(void* start, u32 size, Heap* root,
                  ExpandingHeapAllocator* allocator);

    /* Defined as an empty real destructor in the independently enrolled D1 and
       D0 files. CodeWarrior supplies both this vptr transition and Heap's
       teardown; objisolate retains the lifecycle variant owned by each file. */
    virtual ~ExpandingHeap();

    virtual void  VDestroy();                       /*  2 */
    virtual void* VAllocate(u32 size, int align);   /*  3 */
    virtual void  VDeallocate(void* ptr);           /*  4 */
    virtual void  VDeallocateAll();                 /*  5 */
    virtual bool  VIntact();                        /*  6 */
    virtual void  VRescue();                        /*  7 */
    virtual u32   VReallocate(void* ptr, u32 size); /*  8 */
    virtual u32   VSizeof(void* ptr);               /*  9 */
    virtual u32   VMaxAllocationUnitSize();         /* 10 */
    virtual u32   VMaxAllocatableSize();            /* 11 */
    virtual u32   VMemoryLeft();                    /* 12 */
    virtual u32   VSetNodeID(u32 id);               /* 13 */
    virtual u32   VGetNodeID();                     /* 14 */
    virtual u32   VResizeToFit();                   /* 15 */
};
#endif

#endif
