/* Hand-edited, against evidence. This file used to carry the
 * "AUTO-GENERATED ... by tools/gen_header.py" banner, which was never true --
 * see notes/runbook-type-reconstruction.md section 2.
 *
 * class Heap: 21 matched functions, 5 fields, 16 vtable slots.
 *
 * THE __cplusplus BLOCK BELOW HAD NEVER BEEN COMPILED, and carried the same
 * generator defect as ExpandingHeapAllocator.h and SolidHeapAllocator.h: it
 * emitted `struct a; struct a_; struct b;' as forward declarations -- parameter
 * names mistaken for type names. No source file ever referenced them; dropped.
 *
 * THE ROM'S NAME FOR THIS CLASS IS `mHeap::Heap_t'. Its RTTI record at
 * arm9:0x02099ce4 spells `N5mHeap6Heap_tE', and its two subclasses are
 * `mHeap::SolidHeap_t' (0x02099cf8) and `mHeap::ExpHeap_t' (0x02099cec). The
 * tree calls them Heap / SolidHeap / ExpandingHeap, and every symbol in
 * the config symbols.txt files is spelled that way, so renaming is a whole-tree
 * change and is deliberately NOT done here. Recorded so the next reader does
 * not rediscover it.
 *
 * ==== LAYOUT: 0x14 bytes, and the evidence for each field ====
 *
 * Offsets/widths are observed (evidence_history + evidence_rom, both passes
 * agreeing on 4-byte accesses at 0x00/0x04/0x08/0x0c/0x10). Names come from the
 * constructor's own body, not from a guess: _ZN4HeapC1EPvjP4Heap writes start,
 * size and root into 0x04/0x08/0x0c in that order and then zeroes 0x10.
 *
 * TWO CORRECTIONS TO THE SKELETON THIS REPLACES:
 *
 *   - `u8 unk_010' was WRONG. Both evidence passes see five 4-byte accesses at
 *     0x10 and none narrower; the constructor stores the 32-bit constant 0x4000
 *     there. It is a u32 flag word, and the narrow declaration was the broken
 *     one -- the same shape of error as include/ChainChomp.h's `u8 mScaleX'
 *     (runbook section 2, "Observed is not correct").
 *
 *   - THERE IS NO FIELD AT 0x14 ON Heap. evidence_rom reports one, from a
 *     single witness: the *static* Heap::CreateRootHeap (0x0203c8e8), which
 *     builds an ExpandingHeap in place and therefore writes the derived class's
 *     field through a pointer the pass attributed to Heap. Both derived
 *     constructors settle it -- _ZN9SolidHeapC1E... and
 *     _ZN13ExpandingHeapC1E... each call Heap::Heap(start, size, root), which
 *     writes 0x00..0x10, and only then write their own `allocator' at 0x14. So
 *     0x14 belongs to SolidHeap and ExpandingHeap, which are 0x18 bytes; Heap
 *     is 0x14. The `address-only' evidence at 0x18 is code taking `this + 0x18',
 *     one past a derived object -- CreateRootHeap does exactly that to find the
 *     start of the arena it is about to hand out.
 *
 * ==== THE VTABLE, READ OUT OF THE ROM ====
 *
 * Heap's vtable is at arm9:0x02099d90 and is SIXTEEN slots, of which slots
 * 2..15 are all NULL. Heap is an ABSTRACT BASE. Do not trust
 * `rtti_vtables.py --own Heap', which reports "2 slots": it trims trailing
 * nulls, and for an abstract base the nulls are the whole point. Read the raw
 * words -- the two tables that follow it, SolidHeap (0x02099d48) and
 * ExpandingHeap (0x02099dd8), are full 16-slot tables and fix the order:
 *
 *   slot  0  ~Heap()  (D1)          slot  8  VReallocate
 *   slot  1  ~Heap()  (D0)          slot  9  VSizeof
 *   slot  2  VDestroy               slot 10  VMaxAllocationUnitSize
 *   slot  3  VAllocate              slot 11  VMaxAllocatableSize
 *   slot  4  VDeallocate            slot 12  VMemoryLeft
 *   slot  5  VDeallocateAll         slot 13  VSetNodeID
 *   slot  6  VIntact                slot 14  VGetNodeID
 *   slot  7  VRescue                slot 15  VResizeToFit
 *
 * Return types are taken from the DEFINITIONS (the ExpandingHeap and SolidHeap
 * overrides, which are migrated and byte-gated), never from a caller -- callers
 * disagree 27% of the time and most of these discard the result. Note that
 * slot 3 is spelled `VAllocate(u32, int)': _ZN13ExpandingHeap9VAllocateEji.
 * _ZN9SolidHeap9VAllocateEjj used to claim (u32, u32) for the SAME SLOT, so one
 * of the two imported names was wrong -- and every sibling in the family
 * (Heap::Allocate, SolidHeapAllocator::Allocate,
 * ExpandingHeapAllocator::Allocate) spells `Eji'. The ROM settles it directly:
 * SolidHeapAllocator::Allocate (0x0204eb70) does `cmp r2,#0' then `blt', a
 * SIGNED branch, and negates with `rsb' to allocate backwards -- a u32 would
 * compile to bcc/blo and make that path unreachable. Both VAllocates are
 * four-instruction thunks that forward r1/r2 untouched into it. The SolidHeap
 * symbol has since been corrected to ...Eji; see include/SolidHeap.h.
 *
 * ==== WHERE THE TWO OVERRIDE FAMILIES DISAGREED, AND HOW THE ROM SETTLED IT ====
 *
 * Taking return types from the definitions is only unambiguous where the
 * definitions agree. On two slots they did not, and an earlier revision of this
 * header took ExpandingHeap's side on both for consistency rather than for a
 * reason. Making SolidHeap actually derive from Heap forced the question, and
 * on one of the two the ROM says the other side was right:
 *
 *   slot 13  VSetNodeID  -> u32. NOT void, and this one is BYTE-OBSERVABLE.
 *            _ZN9SolidHeap10VSetNodeIDEj (0x0203c3f0) is eight bytes --
 *            `mov r0,#0 / bx lr'. It materializes a return value, and a void
 *            declaration cannot emit that `mov'. ExpandingHeap's override is a
 *            pure forward, so declaring it u32 costs it nothing: a tail call
 *            leaves the callee's r0 in place either way. Both overrides agree
 *            under u32; only one can be right under void.
 *
 *   slot  9  VSizeof     -> u32, and this one really is unobservable.
 *            SolidHeap's override returns -1 and ExpandingHeap's forwards;
 *            `int' and `u32' both lower the -1 to `mvn r0,#0'. u32 is kept for
 *            consistency with the rest of the slots, and that is the whole
 *            reason -- it is not evidence of a width.
 *
 * The lesson generalizes: a forwarder's return type is unobservable AT ITS OWN
 * DEFINITION and observable AT ITS CALLERS. Heap::Reallocate was declared void
 * on the strength of its definition alone; SolidHeap::VResizeToFit then turned
 * out to branch on its result (`if (!Reallocate(...))'), which void cannot even
 * compile. It returns void*, the moved block, like the slot it forwards to.
 */
#ifndef HEAP_H
#define HEAP_H
#include "types.h"

/* The class is spelled twice, the way include/Fader.h spells Fader: a C
 * translation unit gets no implicit vptr, so the C side must declare `vtable'
 * explicitly or every field below it shifts by 4. Nine .c files include this
 * header, so that is not hypothetical.
 *
 * THE C SPELLING COMES FIRST ON PURPOSE. tools/check_header_offsets.py walks
 * the first `struct X {' it finds and stops at the first `virtual' line,
 * reporting the header "skipped -- polymorphic C++ struct". Putting the C
 * struct first gives that gate a layout it can actually verify: it now reports
 * 5 fields spanning 0x14 and would fail on a wrong offset. With the C++ struct
 * first it reported "0 commented fields ... struct spans 0x0" -- which reads
 * like a pass and checks nothing.
 *
 * FIELD MEANINGS ARE DOCUMENTED ON THE C++ SPELLING BELOW. They are kept off
 * these lines because that gate parses a trailing `/* 0xNN *[/]' and a comment
 * running onto the next line makes the field unparsed. */
#ifndef __cplusplus
struct Heap {
    void* vtable;                   /* 0x00 */
    void* heapStart;                /* 0x04 */
    u32   heapSize;                 /* 0x08 */
    struct Heap* parentHeap;        /* 0x0c */
    u32   flags;                    /* 0x10 */
};
#else
struct Heap {
    void* heapStart;                /* 0x04 -- first byte of the arena this heap
                                       hands out; for a root heap it is the heap
                                       object's own end, `this + 0x18'. */
    u32   heapSize;                 /* 0x08 -- arena size in BYTES, not an end
                                       address. Heap::CreateRootHeap's local
                                       shadow calls it `heapEnd' and types its
                                       own parameter `void* end', but its
                                       mangled name is _ZN4Heap14CreateRootHeapEPvj
                                       -- `j', a u32 -- and it passes
                                       `size - 0x18' onward as a size. Three of
                                       the four hand-written shadows in the tree
                                       already agreed on heapSize; that fourth
                                       one is the outlier. */
    struct Heap* parentHeap;        /* 0x0c -- heap this one was carved out of;
                                       NULL for the root. Heap::Destroy releases
                                       the block back through it. */
    u32   flags;                    /* 0x10 -- bit 0x4000 = fail-fast: when set,
                                       an allocation that returns NULL, a Sizeof
                                       that returns -1, or a ResizeToFit that
                                       returns 0 calls Crash() instead of
                                       reporting failure to the caller. The
                                       constructor sets it, so every heap is
                                       fail-fast unless something clears it. */

    /* THE DESTRUCTOR IS DECLARED FIRST AND MUST NEVER BE DEFINED AS A REAL
       METHOD. It occupies slots 0/1, which makes it this class's key function;
       defining it would emit _ZTV4Heap/_ZTI4Heap/_ZTS4Heap into that
       translation unit and eligible.py would reject the file with "extra
       sections: .data". include/Actor.h states the same rule and for the same
       reason: declaring the destructor first pins that role to translation
       units which by construction never define it. See runbook section 7. */
    virtual ~Heap();

    /* Pure, because the ROM says so: Heap's own sixteen slots hold code only at
       0 and 1, and NULL at 2..15. Every one of these is implemented by
       SolidHeap and by ExpandingHeap. */
    virtual void  VDestroy() = 0;                       /*  2 */
    virtual void* VAllocate(u32 size, int align) = 0;   /*  3 */
    virtual void  VDeallocate(void* ptr) = 0;           /*  4 */
    virtual void  VDeallocateAll() = 0;                 /*  5 */
    virtual bool  VIntact() = 0;                        /*  6 */
    virtual void  VRescue() = 0;                        /*  7 */
    virtual u32   VReallocate(void* ptr, u32 size) = 0; /*  8 -- the new SIZE,
                                                           or 0 on failure; not
                                                           a moved pointer */
    virtual u32   VSizeof(void* ptr) = 0;               /*  9 */
    virtual u32   VMaxAllocationUnitSize() = 0;         /* 10 */
    virtual u32   VMaxAllocatableSize() = 0;            /* 11 */
    virtual u32   VMemoryLeft() = 0;                    /* 12 */
    virtual u32   VSetNodeID(u32 id) = 0;               /* 13 */
    virtual u32   VGetNodeID() = 0;                     /* 14 */
    virtual u32   VResizeToFit() = 0;                   /* 15 */

    /* Non-virtual public interface. Each of these is a thin guard around the
       matching slot: call it, and on failure consult `flags & 0x4000'. Only the
       ones whose definitions are migrated and byte-verified are declared here;
       the rest arrive with their own migrations, where a wrong signature stops
       being unfalsifiable. */
    void* Allocate(u32 size);                /* forwards with align = 4 */
    void* Allocate(u32 size, int align);     /* slot  3, Crash() on NULL */
    void  Deallocate(void* ptr);             /* slot  4 */
    u32   Reallocate(void* ptr, u32 size);   /* slot  8, the new size or 0 */
    int   Sizeof(void* ptr);                 /* slot  9, Crash() on -1 */
    void  Destroy();                         /* slot  2, then releases 0x0c */
    bool  Intact();                          /* slot  6 */
    void  Rescue();                          /* slot  7 */
    u32   MaxAllocationUnitSize();           /* slot 10 */
    void  SetNodeID(u32 id);                 /* slot 13 */
    u32   ResizeToFit();                     /* slot 15, Rescue+Crash on 0 */
    Heap* SetDefault();                      /* installs this as the default
                                                heap, returns the previous one */

    /* STATIC -- and that is read off the argument count, not off the mangled
       name. The Itanium ABI does not encode `this' in the parameter list, so a
       static and a non-static member with the same declared parameters mangle
       identically; include/SolidHeapAllocator.h states the same rule. Each of
       these declares exactly as many parameters as its ROM body takes
       arguments, leaving no room for a leading `this'. The one instance method
       among them, ResizeToFit, is declared above and does use `this'. */
    static Heap* SetupRootHeap();
    static void  InitializeRootHeap();
    static Heap* CreateRootHeap(void* mem, u32 size);
    static Heap* CreateSolidHeap(u32 size, Heap* root, int align);
    static Heap* CreateExpandingHeap(u32 size, Heap* root, int align);
    static void  InitializeGameHeap(u32 size, Heap* root);
    static void* SetupSolidHeapAsDefault(u32 size, Heap* root, int align);
    static void  RestoreFromTemporary();

    /* The two allocator factories. `flags' is the node id the allocator stamps
       on the blocks it hands out; every caller passes 3. Returns NULL when the
       aligned span is too small to hold the allocator's own bookkeeping --
       0x30 bytes for the solid one, 0x4c for the expanding one. */
    static struct SolidHeapAllocator*     CreateSolidHeapAllocator(void* address, u32 size, u32 flags);
    static struct ExpandingHeapAllocator* CreateExpandingHeapAllocator(void* address, u32 size, u32 flags);
};
#endif

#endif
