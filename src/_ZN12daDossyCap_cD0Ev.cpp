//cpp
// @symbol _ZN12daDossyCap_cD0Ev
/* A delete-expression forces mwccarm to materialize daDossyCap_c's deleting
 * destructor and use dActor_c's actor-heap deallocator. */
#include "daDossyCap_c.h"

#ifdef _MSC_VER
/* MSVC needs this flat D0 entry. Call the actual class-body destructor
 * qualified so dispatch is direct, then use the class-specific deallocator.
 * The inline body includes member/base teardown; no separate flat D1 provider
 * is supplied by this branch. The mwccarm definition below is unchanged. */
extern "C" daDossyCap_c *_ZN12daDossyCap_cD0Ev(daDossyCap_c *thiz)
{
    thiz->daDossyCap_c::~daDossyCap_c();          /* direct member/base teardown */
    daDossyCap_c::operator delete(thiz);  /* the class-specific delete D0 ends with */
    return thiz;
}
#else
void daDossyCap_c_EmitDeletingDestructor(daDossyCap_c *cap)
{
    delete cap;
}
#endif
