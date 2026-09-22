//cpp
// @symbol _ZN8PoleLiftD1Ev
/* The class-body destructor is real C++. This otherwise-unused explicit call
 * forces mwccarm to emit its out-of-line D1 copy; objisolate keeps that symbol
 * and discards the forcing wrapper.
 *
 * The body is two vtable stores and three destructor calls, every one a
 * consequence of `struct PoleLift : dBgActor_c`: its own vptr, then
 * dBgActor_c's -- inlined, because dBgActor_c's destructor is defined in its
 * class body -- then dBgActor_c's Model and dBgW_KcMbg, then dActor_c. This
 * class adds no member with a destructor of its own.
 */
#include "PoleLift.h"

void PoleLift_EmitDestructor(PoleLift *p)
{
    p->~PoleLift();
}
