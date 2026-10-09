//cpp
// @symbol _ZN16dPathLiftActor_cD0Ev
/* The deleting (D0) half of the dPathLiftActor_c lifecycle pair. A
 * `delete` expression is what materialises D0 -- destroy through the class and
 * its bases, which is why more than one vptr store appears, then hand the
 * object to the inherited operator delete.
 */
#include "PathLift.h"

#ifdef _MSC_VER
/* MSVC needs this flat D0 entry. Call the actual class-body destructor
 * qualified so dispatch is direct, then use the class-specific deallocator.
 * The inline body includes member/base teardown; no separate flat D1 provider
 * is supplied by this branch. The mwccarm definition below is unchanged. */
extern "C" dPathLiftActor_c *_ZN16dPathLiftActor_cD0Ev(dPathLiftActor_c *thiz)
{
    thiz->dPathLiftActor_c::~dPathLiftActor_c();          /* direct member/base teardown */
    dPathLiftActor_c::operator delete(thiz);  /* the class-specific delete D0 ends with */
    return thiz;
}
#else
void dPathLiftActor_c_EmitDeletingDestructor(dPathLiftActor_c *p)
{
    delete p;
}
#endif
