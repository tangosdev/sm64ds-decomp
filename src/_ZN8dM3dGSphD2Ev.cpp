//cpp
// @symbol _ZN8dM3dGSphD2Ev
/* D2, not D1. This file carries the same definition as
 * src/_ZN8dM3dGSphD1Ev.cpp, and deliberately so: dM3dGSph has no virtual
 * bases, so mwcc emits D1 and D2 as byte-identical code. Only the way the
 * ROM REACHES an address separates them -- a vtable slot holds D1, a derived
 * destructor's base-chain `bl` reaches D2 -- so comparing the two bodies
 * proves nothing and the binding in config/arm9/delinks.txt is what decides.
 * objisolate keeps the D2 variant; the C2/D0/D1 siblings stay in their own
 * files. */
#include "dM3dGSph.h"

dM3dGSph::~dM3dGSph()
{
}
