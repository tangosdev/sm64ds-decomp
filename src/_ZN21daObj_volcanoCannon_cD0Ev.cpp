//cpp
// @symbol _ZN21daObj_volcanoCannon_cD0Ev
/* A delete expression forces the deleting destructor. dActor_c's inline
 * operator delete supplies the actor-heap deallocation used by the ROM. */
#include "daObj_volcanoCannon_c.h"

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC NEVER EMITS IT. ~daObj_volcanoCannon_c()
 * is defined in the class body and the port has no flat D1 for this class
 * (its slot 16 reaches the destructor by MSVC's own name), so this arm
 * spells the D0 the way the ROM's own D0 body is built: the whole
 * destructor, then the class-specific operator delete. MSVC expands the
 * qualified call in place. Nothing here reaches mwccarm: it builds the
 * `#else` arm and emits the ROM bytes it always emitted, and the object is
 * byte-identical either way. */
extern "C" daObj_volcanoCannon_c *_ZN21daObj_volcanoCannon_cD0Ev(daObj_volcanoCannon_c *thiz)
{
    thiz->daObj_volcanoCannon_c::~daObj_volcanoCannon_c();  /* the D1 body, expanded in place */
    daObj_volcanoCannon_c::operator delete(thiz);           /* the class-specific delete D0 ends with */
    return thiz;
}
#else
void VolcanoFire_EmitDeletingDestructor(daObj_volcanoCannon_c *fire)
{
    delete fire;
}
#endif
