//cpp
// @symbol _ZN13PrincessPeachD0Ev
/* A delete expression forces the compiler-spelled deleting destructor. */
#include "PrincessPeach.h"

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC NEVER EMITS IT. ~PrincessPeach()
 * is defined in the class body and the port has no flat D1 for this class
 * (its slot 16 reaches the destructor by MSVC's own name), so this arm
 * spells the D0 the way the ROM's own D0 body is built: the whole
 * destructor, then the class-specific operator delete. MSVC expands the
 * qualified call in place. Nothing here reaches mwccarm: it builds the
 * `#else` arm and emits the ROM bytes it always emitted, and the object is
 * byte-identical either way. */
extern "C" PrincessPeach *_ZN13PrincessPeachD0Ev(PrincessPeach *thiz)
{
    thiz->PrincessPeach::~PrincessPeach();  /* the D1 body, expanded in place */
    PrincessPeach::operator delete(thiz);   /* the class-specific delete D0 ends with */
    return thiz;
}
#else
void PrincessPeach_EmitDeletingDestructor(PrincessPeach *peach)
{
    delete peach;
}
#endif
