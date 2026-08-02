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
 * KEY FUNCTION. CW 1.2 emits the vtable into the TU that defines the first
 * non-inline virtual declared in the class, and that copy collides with the one
 * the module's gap object supplies from ROM data. include/ActorBase.h has to
 * work around this by keeping InitResources out of the class entirely, because
 * ActorBase is the root: all of its virtuals are new, so declaration order sets
 * the slot indices and the destructor is pinned to 16/17.
 *
 * ActorDerived is not constrained that way. An override takes its base's slot
 * whatever order it is declared in, so the destructor can be declared FIRST and
 * become the key function. Nothing defines ~ActorDerived as a C++ method -- the
 * destructor translation units are C files that never see this class -- so no
 * TU is the key function's definition and no vtable is emitted. That is what
 * lets AfterInitResources be a real method here.
 *
 * The rule generalises: only the root class pays the key-function tax.
 */
struct ActorDerived : ActorBase {
    /* Declared first, deliberately -- see KEY FUNCTION above. Overrides slots
       16 (D1) and 17 (D0); the position in this list does not affect that. */
    virtual ~ActorDerived();

    /* slot 2 -- marks the actor for destruction when init failed, then chains. */
    virtual void AfterInitResources(u32 vfSuccess);

    /* Static: no `this`. In the ROM this is a 0xc-byte veneer that tail-calls
       0x02042ffc (ldr ip,[pc]; bx ip; .word), so it has no body of its own. */
    static void Spawn(u32 actorID, ActorBase *parent, int a, int b);
};

#endif
