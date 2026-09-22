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

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S THREE SYMBOLS, AND MSVC ONLY MAKES ONE. It folds
 * the Itanium D1/D0/D2 triple into a single `??1BlendModelAnim@@QAE@XZ`, so the files
 * that carry this one definition would each define that same symbol and the
 * host link refuses them -- measured, LNK2005 twice over for the ModelBase
 * triple alone. The D1 file keeps the definition; this file spells out, in
 * terms of it, what the variant this file is enrolled for actually does.
 * Nothing here reaches mwccarm: it builds the `#else` arm and emits the ROM
 * bytes it always emitted, and the object is byte-identical either way. */
extern "C" BlendModelAnim *_ZN14BlendModelAnimD0Ev(BlendModelAnim *thiz)
{
    thiz->~BlendModelAnim();                /* the D1 body, through the one host symbol */
    BlendModelAnim::operator delete(thiz);  /* the class-specific delete D0 ends with */
    return thiz;
}
#else
BlendModelAnim::~BlendModelAnim()
{
    if (unk_6c)
        func_0203cbc0(unk_6c);
}
#endif
