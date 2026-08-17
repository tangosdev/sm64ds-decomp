#ifndef ACTORDERIVED_H
#define ACTORDERIVED_H

#include "ActorBase.h"

/* The middle link of the actor hierarchy: ActorBase -> ActorDerived -> Actor.
 * Actor is NOT a direct child of ActorBase. See notes/actor-vtables.md.
 *
 * _ZTV12ActorDerived (0x0208e4b8) is ActorBase's 18-slot table with exactly one
 * functional override -- slot 2, AfterInitResources -- plus its own D1/D0 at
 * slots 16/17. Every other slot still points at the ActorBase implementation.
 * The class therefore adds NO new virtuals, which has a useful consequence.
 *
 * KEY FUNCTION. CW 1.2 emits the vtable into the TU that DEFINES the first
 * non-inline virtual declared in the class, and that copy collides with the one
 * the module's gap object supplies from ROM data.
 *
 * The rule, stated precisely: the key function must never be defined as a real
 * method in any translation unit. Its declaration in the class is required and
 * harmless -- include/ActorBase.h does declare InitResources (slot 0) in-class,
 * and removing it would delete a slot and shift the other seventeen. What
 * ActorBase.h relies on is that src/_ZN9ActorBase13InitResourcesEv.cpp defines
 * it as an extern "C" free function rather than a method.
 *
 * ActorDerived used to get there cheaply: the destructor was declared FIRST,
 * making it the key function, and it was never defined as a method, so no TU
 * was the key function's definition. tools/objisolate.py has since made a
 * key-function TU eligible anyway -- it drops the vtable the TU emits and
 * rebinds the reference to the ROM's own _ZTV symbol -- so that is no longer
 * what protects anything. See include/ModelBase.h.
 *
 * THE DESTRUCTOR IS DEFINED INLINE, AND THAT IS LOAD-BEARING FOR SUBCLASSES.
 * Scene::~Scene in the ROM stores two vptrs and then calls ActorBase's
 * destructor directly:
 *
 *     str r2, [r4]        ; _ZTV5Scene
 *     str r1, [r4]        ; _ZTV12ActorDerived   <- this destructor, INLINED
 *     bl  ActorBase::~ActorBase
 *
 * A merely DECLARED `virtual ~ActorDerived();` cannot produce that: the
 * compiler has no body to inline and emits `bl _ZN12ActorDerivedD2Ev` instead,
 * one store where the ROM has two. So the original source defined it in the
 * class body, and every derived destructor inlined it. Moving this definition
 * back out of the class un-matches Scene, Stage and every actor destructor
 * below them.
 *
 * The cost is that src/_ZN12ActorDerivedD1Ev.cpp can no longer define it --
 * that would be a redefinition -- and a TU that merely includes this header
 * emits nothing. That file therefore carries a forcing function instead; see
 * the note in it.
 */
struct ActorDerived : ActorBase {
    /* Declared first, deliberately -- see KEY FUNCTION above. Overrides slots
       16 (D1) and 17 (D0); the position in this list does not affect that.
       DEFINED INLINE on purpose: subclass destructors inline it. */
    virtual ~ActorDerived() {}

    /* slot 2 -- marks the actor for destruction when init failed, then chains. */
    virtual void AfterInitResources(u32 vfSuccess);

    /* Static: no `this`. In the ROM this is a 0xc-byte veneer that tail-calls
       0x02042ffc (ldr ip,[pc]; bx ip; .word), so it has no body of its own.

       It RETURNS the actor it built, and a veneer is exactly the shape that
       cannot say so: three words of `ldr ip,[pc]; bx ip; .word` evidence no
       return value at all. The evidence is at its caller -- Actor::Spawn's
       `bl` is followed straight by the epilogue, so r0 flows out untouched.
       This read `void` until that function was migrated, which is the general
       rule for a forwarder: its return type is unobservable at its definition
       and observable only where it is called. */
    static ActorBase *Spawn(u32 actorID, ActorBase *parent, int a, int b);
};

/* ActorDerived adds no members -- it exists to carry one overridden slot -- so it
   is exactly ActorBase's 0x50. Asserting it holds that claim, and lets
   tools/check_header_offsets.py check everything below it. */
typedef char ActorDerived_size_must_be_0x50[sizeof(ActorDerived) == 0x50 ? 1 : -1];

#endif
