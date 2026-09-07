//cpp
// @symbol _ZN5Crate15OnGroundPoundedER8dActor_c
// recovered name: Crate_OnGroundPounded
/* recovered: renamed to Class_Method, real C++ method */
/* Crate::OnGroundPounded (slot 21, include/dActor_c.h) - recovered from vtable
 * slot identity. Ground-pounding a Crate just calls its own Kill (slot 31);
 * the `dActor_c &other` parameter is unused, matching the ROM body -- it
 * loads the vtable slot and calls through it without ever touching r1. */
#include "Crate.h"

void Crate::OnGroundPounded(dActor_c &other)
{
    Kill();
}
