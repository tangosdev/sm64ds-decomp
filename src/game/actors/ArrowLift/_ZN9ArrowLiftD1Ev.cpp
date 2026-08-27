//cpp
// @symbol _ZN9ArrowLiftD1Ev
/* The class-body destructor is real C++. This otherwise-unused explicit call
 * forces mwccarm to emit its out-of-line D1 copy; objisolate keeps that symbol
 * and discards the forcing wrapper. */
#include "ArrowLift.h"

void ArrowLift_EmitDestructor(ArrowLift *p)
{
    p->~ArrowLift();
}
