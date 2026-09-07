//cpp
// @symbol _ZN4dBgW11GetVelocityER7Vector3
#include "dBgW.h"

/* A non-moving collider has no velocity. The chained stores mirror the
   ROM's z-then-y-then-x order. */
void dBgW::GetVelocity(Vector3 &res)
{
    res.z = 0;
    res.y = res.z;
    res.x = res.y;
}
