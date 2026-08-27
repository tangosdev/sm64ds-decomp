//cpp
// @symbol _ZN17daObjKm3_Kuruma_cD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * THREE vtable stores, matching the sibling include/RickshawPlatformBdw.h
 * documents for daObjKm1_Kuruma_c: `struct daObjKm3_Kuruma_c : daObjKuruma_c :
 * dBgActor_c` emits its own vptr, then daObjKuruma_c's -- inlined, because that
 * destructor is defined in its class body -- then dBgActor_c's, then dBgActor_c's
 * dBgW_KcMbg and Model, then dActor_c. Nothing in the chain adds a member
 * with a destructor.
 */
#include "daObjKm3_Kuruma_c.h"

daObjKm3_Kuruma_c::~daObjKm3_Kuruma_c()
{
}
