//cpp
/* Manually curated shadow translation unit.
 * ov036/daObjRc_Dorifu_c  (4 function(s))
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
 *   [0] 0x02111f8c  src/game/actors/daObjRc_Dorifu_c/_ZN16daObjRc_Dorifu_cD1Ev.cpp
 *   [1] 0x0211200c  src/_ZN16daObjRc_Dorifu_cD0Ev.c
 *   [2] 0x021120a0  src/game/actors/daObjRc_Dorifu_c/_ZN16daObjRc_Dorifu_c16CleanupResourcesEv.cpp
 *   [3] 0x021120b4  src/game/actors/daObjRc_Dorifu_c/_ZN16daObjRc_Dorifu_c13InitResourcesEv.cpp
 */

#include "daObjRc_Dorifu_c.h"

struct ResourceDescriptor {
    void *entries[3];
};

extern "C" {
int func_ov002_020b4b6c(daObjRc_Dorifu_c *self,
                        ResourceDescriptor *descriptor);
int func_ov002_020b4d58(daObjRc_Dorifu_c *self,
                        ResourceDescriptor *descriptor);
extern ResourceDescriptor data_ov036_02113e88;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN16daObjRc_Dorifu_c13InitResourcesEv, 0x021120b4, size 0x14 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjRc_Dorifu_c13InitResourcesEv
int daObjRc_Dorifu_c::InitResources()
{
    return func_ov002_020b4d58(this, &data_ov036_02113e88);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN16daObjRc_Dorifu_c16CleanupResourcesEv, 0x021120a0, size 0x14 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjRc_Dorifu_c16CleanupResourcesEv
int daObjRc_Dorifu_c::CleanupResources()
{
    return func_ov002_020b4b6c(this, &data_ov036_02113e88);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- _ZN16daObjRc_Dorifu_cD1Ev, 0x02111f8c, size 0x80 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjRc_Dorifu_cD1Ev
/* The compiler emits D2, D0 and D1 from this one class definition. */
daObjRc_Dorifu_c::~daObjRc_Dorifu_c()
{
}
