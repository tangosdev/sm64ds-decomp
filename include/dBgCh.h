#ifndef DBGCH_H
#define DBGCH_H

#include "types.h"

/* The collision-query root, vtable _ZTV5dBgCh at 0x020991d8.
 *
 * VTABLE, 2 slots, read out of the ROM:
 *
 *   slot 0  0x02035504  ~dBgCh (D1)
 *   slot 1  0x020354e0  ~dBgCh (D0)
 *
 * Exactly two: the word after slot 1 is 0x0209a764, the __si_class_type_info
 * kind pointer that opens dBgCh_Actr's typeinfo record, not a third slot.
 * The base-object destructor D2 is 0x020354d0, reached by direct call from
 * derived destructors and present in no vtable -- the #774 shape. The
 * constructor (C2) is 0x02035514.
 *
 * A ROOT. Its typeinfo at 0x020991c8 is the 8-byte __class_type_info kind,
 * which carries no base pointer, and the ROM's own name for the class is
 * dBgCh. dBgCh_Actr is the __si_class_type_info record based on it.
 *
 * THE DESTRUCTOR IS DECLARED FIRST -- it is the key function, so the one TU
 * that defines it (src/engine/collision/d_bg_ch.cpp) owns the vtable and its
 * RTTI.
 *
 * LAYOUT is 0x10 bytes, pinned by the constructor:
 *
 *   str  r2,[r0]        the vptr
 *   strb r1,[r0,#4]     = 1, a BYTE -- the detect-flag bitmask
 *   str  r1,[r0,#0xc]   = 0
 *   mvn  r1,#0 / str    [r0,#8] = -1
 *
 * mDetectFlags bits, as ShouldPassThroughImpl (src/engine/collision/d_bg_w.cpp)
 * reads them against a surface's CLPS: 1 = ordinary surfaces,
 * 2 = water (StartDetectingWater), 8 = toxic (StartDetectingToxic); 4, 0x20
 * and 0x40 gate further surface classes whose names are not settled.
 *
 * 0x08/0x0c are the querying actor: func_020353b0 (every SetObjAnd* entry
 * point) stores the actor and its fBase_c uniqueID there, or (0, -1) for no
 * actor -- the same pair, written the same way, that dBgW names
 * owner/ownerUniqueID (func_020395fc). func_02035354 matches both words
 * against a collider's owner so a query skips its own actor's colliders.
 * The m prefix keeps them apart from dBgPi::owner, which every derived query
 * also inherits and which names the HIT collider's owner.
 *
 * ShouldPassThroughImpl is STATIC -- but NOT because of the mangled name.
 * Itanium never encodes `this' in a mangled signature, so
 * _ZN5dBgCh21ShouldPassThroughImplEPvRK4CLPSRKS_b spells the identical string
 * whether the member is static or not. It cannot settle the question, and an
 * earlier version of this comment claimed it could.
 *
 * The evidence that DOES settle it is the argument count. The matched body,
 * in src/engine/collision/d_bg_w.cpp, byte-matches with
 * FOUR parameters in r0-r3 and reads no stack argument. A non-static member
 * with the four parameters the name spells -- (void *, const CLPS &,
 * const dBgCh &, bool) -- would pass five values (this first) and spill the
 * bool to the stack. Four registers, four parameters, no this: static.
 * Same shape as the dBgW UpdatePos callbacks.
 */

#ifdef __cplusplus

#include "types.h"

extern "C" void _ZN6Memory16operator_delete2EPv(void *);

struct CLPS;
struct dActor_c;

struct dBgCh {
    /* 0x00 is the vptr, placed implicitly by the first virtual declaration. */
    u8 mDetectFlags; /* 0x04 - detect-flag bitmask, ctor sets it to 1 */
    u8 pad_005[0x3];
    s32 mOwnerUniqueID; /* 0x08 - owner's fBase_c uniqueID; -1 = none */
    dActor_c *mOwner;   /* 0x0c - the querying actor; 0 = none */

    /* --- vtable, in ROM order. Do not reorder. --- */
    virtual ~dBgCh();        /* slots 0 (D1), 1 (D0) */

    /* WHAT LETS A REAL `~Class()` REPRODUCE THE ROM'S DELETING DESTRUCTOR.
       The compiler generates D0 as "run the destructor body, then call
       operator delete on the class". Without this it emits the global
       `_ZdlPv`, which exists nowhere in this image, and the D0 comes out one
       relocated word different from the ROM -- a difference build_pin.verify
       CANNOT SEE, because it wildcards relocated words. Only the link
       catches it.

       This family deallocates through Memory::operator_delete2, not the
       actor heap: every D0 below ends with a call to 0x0203cbcc. dActor_c's
       copy of this member calls Memory::Deallocate instead, which is why
       each needs its own.

       Inline, and in the IMMEDIATE base -- mwcc inlines it only when it
       finds it in the class or one level up, as include/dActor_c.h records.
       No layout effect: a non-virtual inline member adds no field and no
       vtable slot. */
    void operator delete(void *ptr) { _ZN6Memory16operator_delete2EPv(ptr); }

    /* --- non-virtual --- */
    /* DECLARED, never defined in this header; the definition lives in
       src/engine/collision/d_bg_ch.cpp (the ROM kept only C2 --
       nothing constructs a standalone dBgCh). Declaring it is what makes
       derived constructors emit `bl _ZN5dBgChC2Ev' instead of synthesising
       the base step inline -- notes/ctor-migration.md section 2. */
    dBgCh();

    /* STATIC -- see the header note above: four parameters in r0-r3 and no
       stack argument is what settles it, not the mangled name.

       The return type is not part of an Itanium mangled name, so it is free to
       choose -- and MEASURED either way: `int' and `bool' both reproduce the
       body byte for byte, because every return is already 0 or 1 and mwccarm
       emits no normalising instruction. `bool' is the one that says what the
       function means. */
    static bool ShouldPassThroughImpl(void *p, const CLPS &clps, const dBgCh &bg, bool flag);

    void StartDetectingToxic();
    void StartDetectingWater();
    void StopDetectingOrdinary();
    void StopDetectingWater();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dBgCh_size_must_be_0x10[sizeof(dBgCh) == 0x10 ? 1 : -1];
#endif

#else

struct dBgCh {
    void **vtable;          /* 0x00 */
    u8 mDetectFlags;        /* 0x04 */
    u8 pad_005[0x3];
    s32 mOwnerUniqueID;      /* 0x08 */
    struct dActor_c *mOwner; /* 0x0c */
};

#endif /* __cplusplus */

#endif /* DBGCH_H */
