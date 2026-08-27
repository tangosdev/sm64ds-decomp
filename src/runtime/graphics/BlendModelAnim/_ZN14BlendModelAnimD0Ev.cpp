//cpp
// @symbol _ZN14BlendModelAnimD0Ev
/* recovered: real C++ deleting destructor -- the compiler emits the whole body
 *
 * D0 is the DELETING destructor: destroy through this class and its base --
 * both vptr stores (_ZTV14BlendModelAnim primary,
 * VTable_Animation_BlendModelAnimThunk at +0x50) -- the conditional free of
 * unk_6c, then the ModelAnim base step, then return the object to the heap.
 * Nobody writes that; the same body the D1 file defines emits D2, D0 and D1
 * together, and objisolate keeps the one this file is bound to. The heap
 * hand-off is the family's inline operator delete -- Memory::operator_delete2
 * -- which the D1 never sees because it is not a deleting destructor.
 */
#include "BlendModelAnim.h"

extern "C" void func_0203cbc0(void *ptr);

BlendModelAnim::~BlendModelAnim()
{
    if (unk_6c)
        func_0203cbc0(unk_6c);
}
