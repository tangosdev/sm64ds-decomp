//cpp
// @symbol _ZN11CrazedCrateD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body.
 * Vtable slot 16: one vtable store, the members in reverse, then
 * ~dActor_c. Was a forcing-destructor scaffold (a local duplicate shadow
 * struct) until CrazedCrate.h became the real class; now it just includes
 * it. */
#include "CrazedCrate.h"

CrazedCrate::~CrazedCrate()
{
}
