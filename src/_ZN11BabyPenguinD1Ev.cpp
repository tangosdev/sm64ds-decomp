//cpp
// @symbol _ZN11BabyPenguinD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body.
 * Vtable slot 16: one vtable store, the members in reverse, then
 * ~dActor_c. Was a forcing-destructor scaffold (a local duplicate shadow
 * struct) until BabyPenguin.h became the real class; now it just includes
 * it. */
#include "BabyPenguin.h"

BabyPenguin::~BabyPenguin()
{
}
