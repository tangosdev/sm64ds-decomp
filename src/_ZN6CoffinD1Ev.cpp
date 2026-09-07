//cpp
// @symbol _ZN6CoffinD1Ev
/* The class-body destructor is real C++. This otherwise-unused explicit call
 * forces mwccarm to emit its out-of-line D1 copy; objisolate keeps that symbol
 * and discards the forcing wrapper. */
#include "Coffin.h"

void Coffin_EmitDestructor(Coffin *p)
{
    p->~Coffin();
}
