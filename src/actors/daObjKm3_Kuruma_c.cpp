//cpp
/* Manually curated shadow translation unit.
 * ov047/daObjKm3_Kuruma_c  (4 function(s))
 *
 * NOT ENROLLED and NOT CANONICAL. The readable class and member definitions
 * are compiled only by tubuild's scratch pipeline while the four legacy
 * production sources remain authoritative.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function in reverse source order. The destructor
 * variants are the documented compiler-chosen exception.
 *
 * Assembled from these legacy one-function sources (ROM address order):
 *   [0] 0x021113f8  src/_ZN17daObjKm3_Kuruma_cD1Ev.cpp
 *   [1] 0x02111448  src/_ZN17daObjKm3_Kuruma_cD0Ev.c
 *   [2] 0x021114ac  src/_ZN17daObjKm3_Kuruma_c16CleanupResourcesEv.cpp
 *   [3] 0x021114c0  src/_ZN17daObjKm3_Kuruma_c13InitResourcesEv.cpp
 */

#include "daObjKm3_Kuruma_c.h"

struct ResourceDescriptor {
    void *entries[3];
};

extern "C" {
int func_ov002_020b68b0(daObjKm3_Kuruma_c *self,
                        ResourceDescriptor *descriptor);
int func_ov002_020b6958(daObjKm3_Kuruma_c *self,
                        ResourceDescriptor *descriptor);
extern ResourceDescriptor data_ov047_02112408;
}

/* ROM ordinal 3 -- _ZN17daObjKm3_Kuruma_c13InitResourcesEv */
// @symbol _ZN17daObjKm3_Kuruma_c13InitResourcesEv
int daObjKm3_Kuruma_c::InitResources()
{
    return func_ov002_020b6958(this, &data_ov047_02112408);
}

/* ROM ordinal 2 -- _ZN17daObjKm3_Kuruma_c16CleanupResourcesEv */
// @symbol _ZN17daObjKm3_Kuruma_c16CleanupResourcesEv
int daObjKm3_Kuruma_c::CleanupResources()
{
    return func_ov002_020b68b0(this, &data_ov047_02112408);
}

/* ROM ordinal 0 -- _ZN17daObjKm3_Kuruma_cD1Ev */
// @symbol _ZN17daObjKm3_Kuruma_cD1Ev
/* The compiler emits D2, D0 and D1 from this one class definition. */
