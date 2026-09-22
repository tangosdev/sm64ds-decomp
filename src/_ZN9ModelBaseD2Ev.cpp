//cpp
// @symbol _ZN9ModelBaseD2Ev
/* D2, the base-object destructor. Same definition as the D1 file, and
 * deliberately so: ModelBase has no virtual bases, so mwcc emits D1 and D2 as
 * byte-identical code. Only how the ROM REACHES an address separates them --
 * a vtable slot holds D1, a derived destructor's base-chain `bl` reaches D2. */
#include "ModelBase.h"

extern "C" void Deallocate(void *ptr);

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S THREE SYMBOLS, AND MSVC ONLY MAKES ONE. It folds
 * the Itanium D1/D0/D2 triple into a single `??1ModelBase@@QAE@XZ`, so the files
 * that carry this one definition would each define that same symbol and the
 * host link refuses them -- measured, LNK2005 twice over for the ModelBase
 * triple alone. The D1 file keeps the definition; this file spells out, in
 * terms of it, what the variant this file is enrolled for actually does.
 * Nothing here reaches mwccarm: it builds the `#else` arm and emits the ROM
 * bytes it always emitted, and the object is byte-identical either way. */
extern "C" ModelBase *_ZN9ModelBaseD2Ev(ModelBase *thiz)
{
    thiz->~ModelBase();                /* D2 is the body alone: no delete */
    return thiz;
}
#else
ModelBase::~ModelBase()
{
    if (modelFile != 0) {
        Deallocate(modelFile);
    }
}
#endif
