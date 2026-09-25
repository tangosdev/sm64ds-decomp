//cpp
// @symbol _ZN16dScMgPachinko2_cD0Ev
#include "dScMgPachinko2_c.h"
#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC ONLY MAKES ONE DESTRUCTOR.
 * It folds the Itanium D1/D0 pair into the single ??1dScMgPachinko2_c that
 * src/_ZN16dScMgPachinko2_cD1Ev.cpp defines, so this file cannot carry the
 * same definition (LNK2005). This arm spells the ROM's own D0 instead, the
 * three steps its body makes (overlay 6, 0x020ff444, 0x38 bytes): the store of
 * this class's own vtable, the word its literal pool names
 * (data_ov006_0213dbbc); the base destructor, by the flat ROM name its bl
 * reaches (_ZN11dScMgBase_cD2Ev); then dScMgBase_c's inline operator delete,
 * Memory::Deallocate with the game heap word at 0x020a0eac. Those are the calls
 * port/unmatched/MgPachinko2_Dtor.cpp's host copy made, which forwards here
 * now. Its sibling src/_ZN15dScMgPachinko_cD0Ev.cpp calls the D1 body
 * qualified instead; that path reaches the base through MSVC's own
 * ??1dScMgBase_c, and this body is the port's one caller of the flat base
 * destructor, which would then fall out of the link. Nothing here reaches
 * mwccarm: it builds the `#else` arm and emits the ROM bytes it always
 * emitted, and the object is byte-identical either way. */
extern "C" void _ZN11dScMgBase_cD2Ev(void *self);
extern "C" int data_ov006_0213dbbc[];
extern "C" dScMgPachinko2_c *_ZN16dScMgPachinko2_cD0Ev(dScMgPachinko2_c *thiz)
{
    *(int **)thiz = data_ov006_0213dbbc;      /* this class's own vtable, as the ROM stores it */
    _ZN11dScMgBase_cD2Ev(thiz);               /* the base destructor, by its flat ROM name */
    dScMgPachinko2_c::operator delete(thiz);  /* the class-specific delete D0 ends with */
    return thiz;
}
#else
/* dScMgPachinko2_c::~dScMgPachinko2_c (D0, deleting destructor) --
   dScMgBase_c's own operator delete (its immediate base) covers this; no
   per-class copy needed. */
dScMgPachinko2_c::~dScMgPachinko2_c()
{
}
#endif
