#ifndef DBGCH_H
#define DBGCH_H

#include "types.h"

/* The collision-query root, vtable at 0x020991d8 (still unnamed in
 * symbols.txt; naming it is a separate change).
 *
 * VTABLE, 2 slots, read out of the ROM:
 *
 *   slot 0  0x02035504  ~dBgCh (D1)        - currently func_02035504
 *   slot 1  0x020354e0  ~dBgCh (D0)        - currently func_020354e0
 *
 * Exactly two: the word after slot 1 is 0x0209a764, the __si_class_type_info
 * kind pointer that opens dBgCh_Actr's typeinfo record, not a third slot.
 * The base-object destructor D2 is 0x020354d0, reached by direct call from
 * derived destructors and present in no vtable -- the #774 shape. The
 * constructor is 0x02035514. All four keep their func_ names here; renaming
 * them is a symbols.txt change and does not belong in a byte-neutral pass.
 *
 * A ROOT. Its typeinfo at 0x020991c8 is the 8-byte __class_type_info kind,
 * which carries no base pointer, and the ROM's own name for the class is
 * dBgCh. dBgCh_Actr (dBgCh_Actr) is the __si_class_type_info record based
 * on it.
 *
 * THE DESTRUCTOR IS DECLARED FIRST AND NEVER DEFINED AS A METHOD -- the
 * key-function arrangement from include/ModelBase.h. That is what keeps this
 * translation unit from emitting a vtable of its own.
 *
 * LAYOUT is 0x10 bytes, pinned by the constructor:
 *
 *   str  r2,[r0]        the vptr
 *   strb r1,[r0,#4]     = 1, a BYTE -- the detect-flag bitmask
 *   str  r1,[r0,#0xc]   = 0
 *   mvn  r1,#0 / str    [r0,#8] = -1
 *
 * mDetectFlags keeps its placeholder name because the four matched dBgCh bodies in
 * src/ spell it, and this pass changes no bodies. StartDetectingWater ORs
 * bit 1 into it; the toxic and ordinary calls work the same byte.
 *
 * ShouldPassThroughImpl is STATIC -- but NOT because of the mangled name.
 * Itanium never encodes `this' in a mangled signature, so
 * _ZN5dBgCh21ShouldPassThroughImplEPvRK4CLPSRKS_b spells the identical string
 * whether the member is static or not. It cannot settle the question, and an
 * earlier version of this comment claimed it could.
 *
 * The evidence that DOES settle it is the argument count. The matched body,
 * src/_ZN5dBgCh21ShouldPassThroughImplEPvRK4CLPSRKS_b.c, byte-matches with
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

struct dBgCh {
    /* 0x00 is the vptr, placed implicitly by the first virtual declaration. */
    u8 mDetectFlags;             /* 0x04 - detect-flag bitmask, ctor sets it to 1 */
    u8 pad_005[0x3];
    s32 unk_008;            /* 0x08 - ctor sets -1 */
    s32 unk_00c;            /* 0x0c - ctor sets 0 */

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
    /* DECLARED, never defined in this header; the definition lives in the
       hand-written src/_ZN5dBgChC2Ev.c (the ROM kept only that variant --
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

typedef char dBgCh_size_must_be_0x10[sizeof(dBgCh) == 0x10 ? 1 : -1];

#else

struct dBgCh {
    void **vtable;          /* 0x00 */
    u8 mDetectFlags;             /* 0x04 */
    u8 pad_005[0x3];
    s32 unk_008;            /* 0x08 */
    s32 unk_00c;            /* 0x0c */
};

#endif /* __cplusplus */

#endif /* DBGCH_H */
