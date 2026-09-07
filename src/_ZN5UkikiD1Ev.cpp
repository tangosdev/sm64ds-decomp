//cpp
// @symbol _ZN5UkikiD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body.
 * Vtable slot 16: one vtable store, the members in reverse (PathPtr is
 * trivial and skipped), then ~dActor_c. Was a forcing-destructor scaffold
 * (a local duplicate shadow struct) until Ukiki.h became the real class;
 * now it just includes it. */
#include "Ukiki.h"

Ukiki::~Ukiki()
{
}
