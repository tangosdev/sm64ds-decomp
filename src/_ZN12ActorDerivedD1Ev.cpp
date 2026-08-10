//cpp
// @symbol _ZN12ActorDerivedD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * The hand-written version spelled out the two things a complete-object
 * destructor does anyway: store this class's vtable over the one the base
 * constructor left behind, then run the ActorBase subobject destructor. Both
 * come out of `struct ActorDerived : ActorBase` and `virtual ~ActorDerived()` in the
 * header, so there is nothing left to write.
 *
 * ActorDerived adds no members at all; it exists to sit between ActorBase and
 * the Scene/Actor branches. The whole destructor is the vtable store and the
 * base call.
 */
#include "ActorDerived.h"

ActorDerived::~ActorDerived()
{
}
