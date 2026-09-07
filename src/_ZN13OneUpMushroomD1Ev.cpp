//cpp
// @symbol _ZN13OneUpMushroomD1Ev
/* The destructor is a real method now: the members it used to destroy by hand
   are typed members of OneUpMushroom, so the compiler emits the same chain. See
   include/OneUpMushroom.h for the two witnesses that establish the layout. */
#include "OneUpMushroom.h"

OneUpMushroom::~OneUpMushroom()
{
}
