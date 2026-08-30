#ifndef DBASE_C_H
#define DBASE_C_H

#include "fBase_c.h"

/* The middle link of the actor hierarchy: fBase_c -> dBase_c -> dActor_c.
 * dActor_c is NOT a direct child of fBase_c. See notes/actor-vtables.md.
 *
 * _ZTV7dBase_c (0x0208e4b8) is fBase_c's 18-slot table with exactly one
 * functional override -- slot 2, AfterInitResources -- plus its own D1/D0 at
 * slots 16/17. Every other slot still points at the fBase_c implementation.
 * The class therefore adds NO new virtuals, which has a useful consequence.
 *
 * KEY FUNCTION. AfterInitResources is the first non-inline virtual declared in
 * this class, so defining the real method makes CW 1.2 emit dBase_c's vtable,
 * RTTI, and the out-of-line copies of the inline destructor into the same TU.
 * That is the cartridge's source form: the production ActorDerived TU compiles
 * all five functions in ROM order and verifies the emitted data against its
 * canonical ARM9 homes before discarding the duplicate data contributions.
 *
 * THE DESTRUCTOR IS DEFINED INLINE, AND THAT IS LOAD-BEARING FOR SUBCLASSES.
 * dScene_c::~dScene_c in the ROM stores two vptrs and then calls fBase_c's
 * destructor directly:
 *
 *     str r2, [r4]        ; _ZTV8dScene_c
 *     str r1, [r4]        ; _ZTV7dBase_c   <- this destructor, INLINED
 *     bl  fBase_c::~fBase_c
 *
 * A merely DECLARED `virtual ~dBase_c();` cannot produce that: the
 * compiler has no body to inline and emits `bl _ZN7dBase_cD2Ev` instead,
 * one store where the ROM has two. So the original source defined it in the
 * class body, and every derived destructor inlined it. Moving this definition
 * back out of the class un-matches dScene_c, Stage and every actor destructor
 * below them.
 *
 * There is no second destructor definition and no forcing function. The key
 * function's vtable references cause mwccarm to emit D1 then D0, with no D2,
 * exactly as the ROM records at 0x02013e80..0x02013edc.
 */
struct dBase_c : fBase_c {
    /* Declared first, deliberately -- see KEY FUNCTION above. Overrides slots
       16 (D1) and 17 (D0); the position in this list does not affect that.
       DEFINED INLINE on purpose: subclass destructors inline it. */
    virtual ~dBase_c() {}

    /* slot 2 -- marks the actor for destruction when init failed, then chains. */
    virtual void AfterInitResources(u32 vfSuccess);

    /* Static: no `this`. In the ROM this is a 0xc-byte veneer that tail-calls
       0x02042ffc (ldr ip,[pc]; bx ip; .word), so it has no body of its own.

       It RETURNS the actor it built, and a veneer is exactly the shape that
       cannot say so: three words of `ldr ip,[pc]; bx ip; .word` evidence no
       return value at all. The evidence is at its caller -- dActor_c::Spawn's
       `bl` is followed straight by the epilogue, so r0 flows out untouched.
       This read `void` until that function was migrated, which is the general
       rule for a forwarder: its return type is unobservable at its definition
       and observable only where it is called. */
    static fBase_c *Spawn(u32 actorID, fBase_c *parent, int a, int b);
};

/* dBase_c adds no members -- it exists to carry one overridden slot -- so it
   is exactly fBase_c's 0x50. Asserting it holds that claim, and lets
   tools/check_header_offsets.py check everything below it. */
typedef char dBase_c_size_must_be_0x50[sizeof(dBase_c) == 0x50 ? 1 : -1];

#endif
