//cpp
// @symbol _ZN8BookShotD1Ev
/* The destructor is a real method now: the members it used to destroy by hand
   are typed members of BookShot, so the compiler emits the same chain. See
   include/BookShot.h for the two witnesses that establish the layout. */
#include "BookShot.h"

BookShot::~BookShot()
{
}
