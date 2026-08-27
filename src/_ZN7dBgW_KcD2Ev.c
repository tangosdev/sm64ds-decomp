//cpp
// @symbol _ZN7dBgW_KcD2Ev
/* D2, not D1. This file carries the same definition as
 * src/_ZN7dBgW_KcD1Ev.cpp, and deliberately so: dBgW_Kc has no virtual
 * bases, so mwcc emits D1 and D2 as byte-identical code. Only the way the
 * ROM REACHES an address separates them -- a vtable slot holds D1, a derived
 * destructor's base-chain `bl` reaches D2 -- so comparing the two bodies
 * proves nothing and the binding in config/arm9/delinks.txt is what decides.
 * objisolate keeps the D2 variant; the C2/D0/D1 siblings stay in their own
 * files. */
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
