#ifndef DCC_C_H
#define DCC_C_H

#include "types.h"

/* The cylinder-collision root, vtable _ZTV5dCc_c at 0x0208e6ec
 * (also reachable as data_0208e6ec, kept as an alias so existing consumers
 * and their relocations stay untouched -- the #972 pattern).
 *
 * VTABLE, 4 slots, read out of the ROM:
 *
 *   slot 0  0x020150a8  ~dCc_c (D1)
 *   slot 1  0x0201507c  ~dCc_c (D0)
 *   slot 2  0x00000000  null - pure; GetPos() in every derived table
 *   slot 3  0x00000000  null - pure; GetOwnerID() in every derived table
 *
 * A ROOT, not a derived class. Its typeinfo at 0x0208e660 is the 8-byte
 * __class_type_info kind, which carries no base pointer; the ROM's own name
 * for it is dCc_c. The three children are __si_class_type_info records
 * pointing back at it: dCcPos_c (dCcPos_c), dCcAc_c
 * (dCcAc_c) and, one level further down, dCcAcPos_c
 * (dCcAcPos_c, based on dCcAc_c).
 *
 * THE DESTRUCTOR IS DECLARED FIRST and defined as real C++ in each dedicated
 * lifecycle source. mwccarm emits the D2/D0/D1 variant group from that one
 * definition; objisolate retains the variant enrolled at each ROM address.
 * The destructor body performs the intrusive-list unlink described below,
 * and the class-local operator delete makes D0 use the cartridge's allocator.
 *
 * Unlink (0x02014fa4) IS NOT A BASE DESTRUCTOR, whatever the old comments in
 * the D1/D2 files said. There is no base. It is the intrusive-list unlink
 * that D0/D1/D2 all call, and its body says so:
 *
 *   ldr r2,[r0,#0x28] / strne r1,[r2,#0x2c]   prev->next = next
 *   ldr r2,[r0,#0x2c] / strne r1,[r2,#0x28]   next->prev = prev
 *
 * with the list head at data_0209cee8 patched when this node is the head,
 * then both links zeroed.
 *
 * Update() is the matching link-in: it stores the old head into next, points
 * the old head's prev back at this, and makes this the new head. C2 zeroes
 * both links, which is what an unlinked node looks like.
 *
 * LAYOUT is 0x30 bytes. Offsets come from Init (0x04/0x08/0x18/0x1c), Clear
 * (0x0c/0x10/0x14/0x20/0x24), Update and the unlink above (0x28/0x2c), and
 * the derived classes start their own fields at exactly 0x30.
 *
 * Field NAMES below are the ones Process() had already recovered and carried
 * as a local struct; hoisting them here is what removes that duplicate. The
 * names are a reading, not pinned by the ROM the way the offsets are.
 */

#ifdef __cplusplus

extern "C" void _ZN6Memory16operator_delete2EPv(void *);

struct dCc_c {
    /* Bit meanings below are a best-effort reading, not pinned by the ROM the
     * way the offsets are -- same caveat as the field NAMES note atop this
     * file. Two are proven by our own matched code: bit 0x2000 matches
     * dActor_c::FindEgg's mask, and bit 0x4000 matches
     * dActor_c::FindExplosionActor's. The rest are a plausible reading from
     * the surrounding game mechanics, not independently verified.
     *
     * vulnFlags (which kinds of hit this clsn can register) and hitFlags
     * (which of those actually hit it this frame) share the same bit table:
     *   0x0001 disabled           0x0002 char movement        0x0004 char projectile
     *   0x0008 char body          0x0010 mega character       0x0020 spin/ground-pound
     *   0x0040 punch              0x0080 kick                 0x0100 breakdance
     *   0x0200 slide kick         0x0400 dive                 0x1000 grab
     *   0x2000 egg (PROVEN)       0x4000 explosion (PROVEN)   0x8000 yoshi tongue
     *   0x40000 fire              0x100000 collectable        0x200000 enemy
     *   0x400000 player           0x800000 player interact    0x1000000 tree
     *   0x2000000 handstand */

    /* 0x00 is the vptr, placed implicitly by the first virtual declaration. */
    Fix12i radius;          /* 0x04 - Init arg 1 */
    Fix12i height;          /* 0x08 - Init arg 2 */
    Vector3 pushback;       /* 0x0c - cleared by Clear */
    u32 flags;              /* 0x18 - Init arg 3; bit 0x1 makes Update bail */
    u32 vulnFlags;          /* 0x1c - Init arg 4; see the bit table above */
    u32 hitFlags;           /* 0x20 - cleared by Clear; see the bit table above */
    u32 otherOwner;         /* 0x24 - cleared by Clear */
    dCc_c *prev;     /* 0x28 - intrusive list, zeroed by C2 */
    dCc_c *next;     /* 0x2c - intrusive list, zeroed by C2 */

    /* --- vtable, in ROM order. Do not reorder. --- */
    virtual ~dCc_c();            /* slots 0 (D1), 1 (D0) */
    virtual Vector3 &GetPos() = 0;      /* slot 2 - null in the ROM table */
    virtual u32 GetOwnerID() = 0;       /* slot 3 - null in the ROM table */

    /* Defined as real C++ in src/_ZN5dCc_cC2Ev.cpp. Every derived constructor
     * calls this base step, whose member-initializer list leaves a newly
     * constructed collision node unlinked. */
    dCc_c();

    /* --- non-virtual --- */
    void Init(Fix12i radius, Fix12i height, u32 flags, u32 vulnFlags);
    void Clear();
    /* Removes this node from the global collision list. The role is proven
       by the prev/next rewiring; the original member spelling is not. */
    void Unlink();
    void Update();
    static void Process();

    /* WHAT LETS A REAL `~Class()` REPRODUCE THE ROM'S DELETING DESTRUCTOR.
       The compiler generates D0 as "run the destructor body, then call operator
       delete on the class". Without this it emits the global `_ZdlPv`, which
       exists nowhere in this image, and the D0 comes out one relocated word
       different from the ROM -- a difference build_pin.verify CANNOT SEE,
       because it wildcards relocated words. Only the link catches it.

       This family deallocates through Memory::operator_delete2, not the actor
       heap: every D0 below ends with a call to 0x0203cbcc. dActor_c's copy of this
       member calls Memory::Deallocate instead, which is why each needs its own.

       Inline, and in the IMMEDIATE base -- mwcc inlines it only when it finds it
       in the class or one level up, as include/dActor_c.h records. No layout
       effect: a non-virtual inline member adds no field and no vtable slot. */
    void operator delete(void *ptr) { _ZN6Memory16operator_delete2EPv(ptr); }

};

typedef char dCc_c_size_must_be_0x30[sizeof(dCc_c) == 0x30 ? 1 : -1];

#else

struct dCc_c {
    void **vtable;             /* 0x00 */
    s32 radius;                /* 0x04 */
    s32 height;                /* 0x08 */
    s32 pushbackX;             /* 0x0c */
    s32 pushbackY;             /* 0x10 */
    s32 pushbackZ;             /* 0x14 */
    u32 flags;                 /* 0x18 */
    u32 vulnFlags;             /* 0x1c */
    u32 hitFlags;              /* 0x20 */
    u32 otherOwner;            /* 0x24 */
    struct dCc_c *prev; /* 0x28 */
    struct dCc_c *next; /* 0x2c */
};

#endif /* __cplusplus */

#endif /* DCC_C_H */
