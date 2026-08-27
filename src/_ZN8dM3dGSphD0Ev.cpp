//cpp
// @symbol _ZN8dM3dGSphD0Ev
/* D0, the DELETING destructor. Unlike the D1/D2 pair these are NOT the
 * same code -- D0 runs the destructor and then hands the object to
 * operator delete, so it is longer. What is shared is the SOURCE: one
 * `Class::~Class()` makes mwcc emit D0, D1 and D2 together, and
 * objisolate keeps the one this file is bound to by config/.../delinks.txt.
 * That is why this file carries the same definition as
 * src/_ZN8dM3dGSphD1Ev.cpp -- it is not duplication, it is how
 * one-symbol-per-file enrolment meets a compiler that emits three.
 *
 * The `operator delete` on the immediate base is what makes the length come
 * out right: CW inlines it into D0 only when it finds one there, and without
 * it D0 calls the global _ZdlPv and lands three words short. */
/* And this one deallocates through Memory::operator_delete2, not the actor
 * heap -- see the operator delete in include/dM3dGSph.h and why it has to be
 * there rather than inherited. */
#include "dM3dGSph.h"

dM3dGSph::~dM3dGSph()
{
}
