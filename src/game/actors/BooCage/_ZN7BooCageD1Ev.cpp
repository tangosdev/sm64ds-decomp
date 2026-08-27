//cpp
// @symbol _ZN7BooCageD1Ev
/* The destructor is a real method now: the four members it used to destroy by hand
   are typed members of BooCage, so the compiler emits the same chain. See
   include/BooCage.h for the two witnesses that establish the layout -- both read out
   of the ROM, since this class's factory has never had a C++ source file. */
#include "BooCage.h"

BooCage::~BooCage()
{
}
