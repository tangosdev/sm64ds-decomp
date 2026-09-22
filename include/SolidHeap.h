/* Hand-edited, against evidence. This file used to carry the
 * "AUTO-GENERATED ... by tools/gen_header.py" banner, which was never true --
 * see notes/runbook-type-reconstruction.md section 2.
 *
 * class SolidHeap: 15 matched functions, 1 own field, 16 vtable slots.
 *
 * THE ROM'S NAME FOR THIS CLASS IS `mHeap::SolidHeap_t' (RTTI record at
 * arm9:0x02099cf8, mangled N5mHeap11SolidHeap_tE). The tree calls it SolidHeap
 * and every symbol is spelled that way; see include/Heap.h for why renaming is
 * not done piecemeal.
 *
 * ==== IT DERIVES FROM Heap, AND IT DID NOT SAY SO ====
 *
 * The skeleton this replaces declared a FLAT struct -- `u8 pad_000[0x14]' then
 * the allocator -- and listed its V* methods as ordinary non-virtual members.
 * Both halves were wrong, and the ROM is unambiguous on each:
 *
 *   - The RTTI record at 0x02099cf8 is kind `si' (single inheritance) with its
 *     base pointing at 0x02099ce4, which is Heap.
 *   - The vtable at 0x02099d48 is a full sixteen slots that shadow Heap's
 *     sixteen one for one: D1, D0, then the fourteen V* in Heap's slot order.
 *     Heap's own slots 2..15 are null, so Heap is abstract and these are its
 *     implementations, not incidental methods that happen to share names.
 *   - The constructor _ZN9SolidHeapC1EPvjP4HeapP18SolidHeapAllocator calls
 *     Heap's constructor first and only then writes 0x14, which is exactly a
 *     base subobject followed by a derived field.
 *
 * The `pad_000[0x14]' spelling reproduced the right byte offsets, so nothing
 * ever failed -- it simply could not express that a SolidHeap IS a Heap. Every
 * file that needed the base's fields re-declared them locally, and they drifted:
 * VResizeToFit and VDestroy each carried their own `struct Heap', and VDestroy's
 * copy is still labelled ExpandingHeap in its comments.
 *
 * The generator also emitted `struct a; struct id_;' as forward declarations --
 * parameter names mistaken for type names. No source file referenced them; they
 * are dropped, as they were in ExpandingHeapAllocator.h (#1211) and
 * SolidHeapAllocator.h (#1215).
 *
 * ==== SIGNATURES THAT CHANGED, AND WHY ====
 *
 * Overriding is what makes a signature falsifiable: the base declares the slot,
 * so a mismatched override is a compile error rather than a silent second
 * opinion. Three fell out of that, all with ROM evidence:
 *
 *   VAllocate  was _ZN9SolidHeap9VAllocateEjj -- (u32, u32). Slot 3 is shared
 *              with _ZN13ExpandingHeap9VAllocateEji -- (u32, int) -- so they
 *              could not both be right. SolidHeapAllocator::Allocate
 *              (0x0204eb70), which this override tail-calls with r1/r2
 *              untouched, does `cmp r2,#0' then `blt' -- a SIGNED branch -- and
 *              negates with `rsb' to allocate backwards. A u32 alignment would
 *              compile to bcc/blo and make that path unreachable. The parameter
 *              is `int'; the symbol is renamed to ...Eji to match.
 *
 *   VSetNodeID stays u32 and Heap.h moved to meet it. This override is eight
 *              bytes -- `mov r0,#0 / bx lr' -- so the return value is
 *              materialized and `void' could not emit that `mov'.
 *
 *   VSizeof    becomes u32, from int. Unobservable: it returns -1, and both
 *              spellings lower that to `mvn r0,#0'. Changed only so the family
 *              agrees.
 */
#ifndef SOLIDHEAP_H
#define SOLIDHEAP_H
#include "types.h"
#include "Heap.h"

/* SolidHeapAllocator is only ever pointed at from here, so a declaration is
 * enough -- no definition is pulled in. The typedef keeps the member spelled
 * the same in C and in C++; the guard is common.h's idiom for the same job. */
#ifndef SOLIDHEAPALLOCATOR_FWD_DECLARED
#define SOLIDHEAPALLOCATOR_FWD_DECLARED
struct SolidHeapAllocator;
typedef struct SolidHeapAllocator SolidHeapAllocator;
#endif

#ifndef __cplusplus
/* Flat, because C has neither bases nor an implicit vptr. The first five
   members are Heap's, repeated in its order; see include/Heap.h. */
struct SolidHeap {
    void* vtable;                   /* 0x00 */
    void* heapStart;                /* 0x04 */
    u32   heapSize;                 /* 0x08 */
    struct Heap* parentHeap;        /* 0x0c */
    u32   flags;                    /* 0x10 */
    SolidHeapAllocator* allocator;  /* 0x14 */
};
#else
struct SolidHeap : Heap {
    /* The one field of its own. Everything below 0x14 belongs to the base. */
    SolidHeapAllocator* allocator;  /* 0x14 -- fixed-size linear allocator; it
                                       lives inside the arena it manages, which
                                       is why Heap::CreateSolidHeapAllocator
                                       constructs it at the arena's own start. */

    /* The constructor's initializer list is the ROM body: construct Heap, then
       store the allocator pointer. CodeWarrior owns the intervening vptr store. */
    SolidHeap(void* start, u32 size, Heap* root,
              SolidHeapAllocator* allocator);

    /* D1 and D0 are independently enrolled compiler-owned variants of this
       empty real destructor; Heap's teardown is generated, not hand-called. */
    virtual ~SolidHeap();

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
