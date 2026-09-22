//cpp
// @symbol _ZN12daDossyCap_cD1Ev
/* The inline class-body destructor is real C++. This explicit call forces
 * mwccarm to materialize the complete destructor; objisolate keeps the
 * enrolled D1 and discards the forcing helper and metadata passengers. */
#include "daDossyCap_c.h"

void daDossyCap_c_EmitDestructor(daDossyCap_c *cap)
{
    cap->~daDossyCap_c();
}
