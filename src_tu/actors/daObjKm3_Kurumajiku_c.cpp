//cpp
/* Manually curated shadow translation unit.
 * ov047/daObjKm3_Kurumajiku_c  (4 function(s))
 *
 * NOT ENROLLED and NOT CANONICAL. The readable class and member definitions
 * are compiled only by tubuild's scratch pipeline while the four legacy
 * production sources remain authoritative.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function, in the REVERSE of source order, so
 * the highest-address ROM function is written FIRST here. Do not reorder;
 * see notes/tu-reconstruction-pilot-report.md sec 3 for the one documented
 * exception (a destructor's D0/D1/D2 group has compiler-chosen order).
 *
 * Assembled from these legacy one-function sources (ROM address order):
 *   [0] 0x021111a0  src/_ZN21daObjKm3_Kurumajiku_cD1Ev.cpp
 *   [1] 0x021111f0  src/_ZN21daObjKm3_Kurumajiku_cD0Ev.c
 *   [2] 0x02111254  src/_ZN21daObjKm3_Kurumajiku_c16CleanupResourcesEv.cpp
 *   [3] 0x02111268  src/_ZN21daObjKm3_Kurumajiku_c13InitResourcesEv.cpp
 */

#include "daObjKm3_Kurumajiku_c.h"

struct ResourceDescriptor {
    void *entries[3];
};

extern "C" {
int func_ov002_020b6ac8(daObjKm3_Kurumajiku_c *self,
                        ResourceDescriptor *descriptor);
int func_ov002_020b6c54(daObjKm3_Kurumajiku_c *self,
                        ResourceDescriptor *descriptor, unsigned actorID);
extern ResourceDescriptor data_ov047_02112258;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN21daObjKm3_Kurumajiku_c13InitResourcesEv, 0x02111268, size 0x18 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN21daObjKm3_Kurumajiku_c13InitResourcesEv
/* Delegates to the shared setup with this class's three-word descriptor and
 * the actor ID of the cart it drives. */
int daObjKm3_Kurumajiku_c::InitResources()
{
    return func_ov002_020b6c54(this, &data_ov047_02112258, 0x97);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN21daObjKm3_Kurumajiku_c16CleanupResourcesEv, 0x02111254, size 0x14 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN21daObjKm3_Kurumajiku_c16CleanupResourcesEv
int daObjKm3_Kurumajiku_c::CleanupResources()
{
    return func_ov002_020b6ac8(this, &data_ov047_02112258);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- _ZN21daObjKm3_Kurumajiku_cD1Ev, 0x021111a0, size 0x50 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN21daObjKm3_Kurumajiku_cD1Ev
/* The compiler emits D2, D0 and D1 from this one class definition. */
daObjKm3_Kurumajiku_c::~daObjKm3_Kurumajiku_c()
{
}
