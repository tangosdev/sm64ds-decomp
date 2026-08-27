//cpp
// @symbol _ZN7dBgW_KcD0Ev
/* D0, the DELETING destructor. Unlike the D1/D2 pair these are NOT the
 * same code -- D0 runs the destructor and then hands the object to
 * operator delete, so it is longer. What is shared is the SOURCE: one
 * `dBgW_Kc::~dBgW_Kc()` makes mwcc emit D0, D1 and D2 together, and
 * objisolate keeps the one this file is bound to by config/arm9/delinks.txt.
 * That is why this file carries the same definition as
 * src/_ZN7dBgW_KcD1Ev.cpp -- it is not duplication, it is how
 * one-symbol-per-file enrolment meets a compiler that emits three. */
/* recovered: real C++ destructor -- one release call
 *
 * `clps` at 0x24 is a handle into the collision-parameter table, taken with
 * func_0203821c and given back with func_02038224. Handing it back is the only
 * real statement in these 0x30 bytes; the vtable store and the
 * dBgW subobject call come from the header.
 *
 * The release is still called by address rather than by name -- func_02038224
 * is a 4-byte function nothing in the tree has identified yet -- so it stays an
 * extern "C" declaration. It is passed &clps, not clps: the ROM hands it the
 * ADDRESS of the field.
 */
#include "dBgW_Kc.h"

extern "C" void func_02038224(void *clps);

dBgW_Kc::~dBgW_Kc()
{
    func_02038224(&clps);
}
