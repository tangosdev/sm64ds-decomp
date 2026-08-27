//cpp
// @symbol _ZN9ModelAnimD2Ev
/* D2, not D1. This file carries the same definition as
 * src/_ZN9ModelAnimD1Ev.cpp, and deliberately so: ModelAnim has no virtual
 * bases, so mwcc emits D1 and D2 as byte-identical code. Only the way the
 * ROM REACHES an address separates them -- a vtable slot holds D1, a derived
 * destructor's base-chain `bl` reaches D2 -- so comparing the two bodies
 * proves nothing and the binding in config/arm9/delinks.txt is what decides.
 * objisolate keeps the D2 variant; the C2/D0/D1 siblings stay in their own
 * files. */
/* recovered: real C++ destructor -- defined out of line, D1 bound here
 *
 * ModelAnim.h declares `virtual ~ModelAnim();` but defines it nowhere, so
 * this TU's empty body is what emits the symbol. The compiler then writes
 * the two vptr stores (_ZTV9ModelAnim primary, VTable_Animation_ModelAnimThunk
 * at +0x50), runs the Animation member step and the Model base step -- the
 * same body the ROM carries at 0x0201691c. objisolate keeps only the D1
 * variant this file is bound to; the C2/D0 siblings stay in their own files
 * (notes/dtor-migration.md section 1).
 */
#include "ModelAnim.h"

ModelAnim::~ModelAnim()
{
}
