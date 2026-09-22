//cpp
// @symbol _ZN9ModelBaseD0Ev
/* D0, the DELETING destructor: run the destructor body, then hand the object
 * to operator delete. Longer than the D1/D2 pair, but built from the same one
 * `ModelBase::~ModelBase()` definition. */
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
extern "C" ModelBase *_ZN9ModelBaseD0Ev(ModelBase *thiz)
{
    thiz->~ModelBase();                /* the D1 body, through the one host symbol */
    ModelBase::operator delete(thiz);  /* the class-specific delete D0 ends with */
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
