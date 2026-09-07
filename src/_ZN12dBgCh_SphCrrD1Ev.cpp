//cpp
// @symbol _ZN12dBgCh_SphCrrD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * The hand-written version spelled out what a complete-object destructor does
 * anyway: store this class's vtable over the one the base constructor left,
 * destroy the members in reverse declaration order, then run the base
 * subobject destructor. All of it follows from `struct dBgCh_SphCrr : dBgCh, dBgPi, dM3dGSph` and the member types
 * in the header, so the body is empty and the bytes still reproduce.
 */
#include "dBgCh_SphCrr.h"

dBgCh_SphCrr::~dBgCh_SphCrr()
{
}
