//cpp
// @symbol _ZN13daObjCasket_cD1Ev
/* The class-body destructor is real C++. This otherwise-unused explicit call
 * forces mwccarm to emit its out-of-line D1 copy; objisolate keeps that symbol
 * and discards the forcing wrapper. */
#include "daObjCasket_c.h"

void Coffin_EmitDestructor(daObjCasket_c *p)
{
    p->~daObjCasket_c();
}
