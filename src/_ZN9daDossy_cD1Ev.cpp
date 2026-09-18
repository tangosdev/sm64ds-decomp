//cpp
// @symbol _ZN9daDossy_cD1Ev
/* The inline class-body destructor is real C++. This explicit call forces
 * mwccarm to materialize daDossy_c's complete destructor; objisolate keeps the
 * enrolled D1 and discards the forcing helper. */
#include "daDossy_c.h"

void daDossy_c_EmitDestructor(daDossy_c *p)
{
    p->~daDossy_c();
}
