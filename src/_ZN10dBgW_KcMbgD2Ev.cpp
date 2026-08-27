//cpp
// @symbol _ZN10dBgW_KcMbgD2Ev
/* D2, not D1. This file carries the same definition as
 * src/_ZN10dBgW_KcMbgD1Ev.cpp, and deliberately so: dBgW_KcMbg has no virtual
 * bases, so mwcc emits D1 and D2 as byte-identical code. Only the way the
 * ROM REACHES an address separates them -- a vtable slot holds D1, a derived
 * destructor's base-chain `bl` reaches D2 -- so comparing the two bodies
 * proves nothing and the binding in config/arm9/delinks.txt is what decides.
 * objisolate keeps the D2 variant; the C2/D0/D1 siblings stay in their own
 * files. */
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * The hand-written version spelled out the two things a complete-object
 * destructor does anyway: store this class's vtable over the one the base
 * constructor left behind, then run the base subobject destructor. Both come
 * out of `struct dBgW_KcMbg : dBgW_Kc` and `virtual ~...()` in
 * the header, so there is nothing left to write.
 *
 * None of the members has a destructor -- they are Fix12i, Matrix4x3, Vector3
 * and scalars -- which is why the ROM's 0x24 bytes contain no member calls and
 * the empty body reproduces them.
 */
#include "dBgW_KcMbg.h"

dBgW_KcMbg::~dBgW_KcMbg()
{
}
