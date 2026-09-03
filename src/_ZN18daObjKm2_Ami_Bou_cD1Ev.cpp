//cpp
// @symbol _ZN18daObjKm2_Ami_Bou_cD1Ev
/* The class-body destructor is real C++. This otherwise-unused explicit call
 * forces mwccarm to emit its out-of-line D1 copy; objisolate keeps that symbol
 * and discards the forcing wrapper.
 *
 * The body is two vtable stores and three destructor calls, every one a
 * consequence of `struct daObjKm2_Ami_Bou_c : dBgActor_c`: its own vptr, then
 * dBgActor_c's -- inlined, because dBgActor_c's destructor is defined in its
 * class body -- then dBgActor_c's Model and dBgW_KcMbg, then dActor_c. This
 * class adds no member with a destructor of its own.
 */
#include "daObjKm2_Ami_Bou_c.h"

void daObjKm2_Ami_Bou_c_EmitDestructor(daObjKm2_Ami_Bou_c *p)
{
    p->~daObjKm2_Ami_Bou_c();
}
