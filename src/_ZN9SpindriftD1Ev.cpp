//cpp
// @symbol _ZN9SpindriftD1Ev
/* The destructor is a real method now: the members it used to destroy by hand
   are typed members of Spindrift, so the compiler emits the same chain. See
   include/Spindrift.h for the two witnesses that establish the layout. */
#include "Spindrift.h"

Spindrift::~Spindrift()
{
}
