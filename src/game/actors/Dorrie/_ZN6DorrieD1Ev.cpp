//cpp
// @symbol _ZN6DorrieD1Ev
/* The inline class-body destructor is real C++. This explicit call forces
 * mwccarm to materialize Dorrie's complete destructor; objisolate keeps the
 * enrolled D1 and discards the forcing helper. */
#include "Dorrie.h"

void Dorrie_EmitDestructor(Dorrie *p)
{
    p->~Dorrie();
}
