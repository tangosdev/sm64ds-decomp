//cpp
// @symbol _ZN6ShipUpD1Ev
/* ShipUp's destructor is defined INLINE in include/ShipUp.h, the form that makes
 * mwccarm emit the complete-object copy before the deleting one, as the ROM has
 * them. A class-body destructor has no out-of-line definition of its own, so
 * this otherwise-uncalled helper is what materialises the D1 variant that vtable
 * slot 0 needs an address for; objisolate keeps that symbol and discards the
 * forcing wrapper.
 *
 * The body is two vptr stores and the member teardown that follows from
 * `struct ShipUp : dBgActor_c`: this class's vptr, then dBgActor_c's -- inlined,
 * because dBgActor_c's destructor is defined in its class body -- then
 * dBgActor_c's Model and dBgW_KcMbg, then dActor_c. ShipUp adds no member with
 * a destructor of its own.
 */
#include "ShipUp.h"

void ShipUp_EmitDestructor(ShipUp *p)
{
    p->~ShipUp();
}
