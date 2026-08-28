//cpp
/* Manually curated translation unit, reconstructed and promoted.
 * ov043/daObjKm1_Dorifu_c  (4 function(s))
 *
 * CANONICAL. The readable class and member definitions below are what the ROM
 * build compiles; the four legacy production sources they replaced are gone.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function, in the REVERSE of source order, so
 * the highest-address ROM function is written FIRST here. Do not reorder;
 * see notes/tu-reconstruction-pilot-report.md sec 3 for the one documented
 * exception (a destructor's D0/D1/D2 group has compiler-chosen order).
 *
 * Assembled from these legacy one-function sources (ROM address order):
 *   [0] 0x02111630  src/_ZN17daObjKm1_Dorifu_cD1Ev.cpp
 *   [1] 0x021116b0  src/_ZN17daObjKm1_Dorifu_cD0Ev.c
 *   [2] 0x02111744  src/_ZN17daObjKm1_Dorifu_c16CleanupResourcesEv.cpp
 *   [3] 0x02111758  src/_ZN17daObjKm1_Dorifu_c13InitResourcesEv.cpp
 */

#include "daObjKm1_Dorifu_c.h"

extern daObjDorifuResources data_ov043_02112518[5];

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN17daObjKm1_Dorifu_c13InitResourcesEv, 0x02111758, size 0x14 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN17daObjKm1_Dorifu_c13InitResourcesEv
int daObjKm1_Dorifu_c::InitResources()
{
    return daObjDorifu_c::InitResources(data_ov043_02112518);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN17daObjKm1_Dorifu_c16CleanupResourcesEv, 0x02111744, size 0x14 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN17daObjKm1_Dorifu_c16CleanupResourcesEv
int daObjKm1_Dorifu_c::CleanupResources()
{
    return daObjDorifu_c::CleanupResources(data_ov043_02112518);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- _ZN17daObjKm1_Dorifu_cD1Ev, 0x02111630, size 0x80 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN17daObjKm1_Dorifu_cD1Ev
/* The compiler emits D2, D0 and D1 from this one class definition. */
