//cpp
/* Manually curated shadow translation unit.
 * ov022/daObjFl_Fall_Block_c  (4 function(s))
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
 *   [0] 0x02112380  src/_ZN20daObjFl_Fall_Block_cD1Ev.cpp
 *   [1] 0x021123d0  src/_ZN20daObjFl_Fall_Block_cD0Ev.c
 *   [2] 0x02112434  src/_ZN20daObjFl_Fall_Block_c16CleanupResourcesEv.cpp
 *   [3] 0x02112448  src/_ZN20daObjFl_Fall_Block_c13InitResourcesEv.cpp
 */

#include "daObjFl_Fall_Block_c.h"

struct ResourceDescriptor {
    void *entries[3];
};

extern "C" {
int func_ov098_0213a2cc(daObjFl_Fall_Block_c *self,
                        ResourceDescriptor *descriptor);
int func_ov098_0213a794(daObjFl_Fall_Block_c *self,
                        ResourceDescriptor *descriptor);
extern ResourceDescriptor data_ov022_0211427c;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN20daObjFl_Fall_Block_c13InitResourcesEv, 0x02112448, size 0x14 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN20daObjFl_Fall_Block_c13InitResourcesEv
/* daObjFl_Fall_Block_c::InitResources -- vtable slot 0, one of the two slots
 * this class overrides that daObjFallBlock_c leaves null. The body is a
 * delegation to the shared ov098 falling-block setup helper, handing it the
 * Lethal Lava Land descriptor in ov022. */
int daObjFl_Fall_Block_c::InitResources()
{
    return func_ov098_0213a794(this, &data_ov022_0211427c);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN20daObjFl_Fall_Block_c16CleanupResourcesEv, 0x02112434, size 0x14 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN20daObjFl_Fall_Block_c16CleanupResourcesEv
/* daObjFl_Fall_Block_c::CleanupResources -- vtable slot 3, the teardown half of
 * the InitResources delegation, over the same ov022 descriptor. */
int daObjFl_Fall_Block_c::CleanupResources()
{
    return func_ov098_0213a2cc(this, &data_ov022_0211427c);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- _ZN20daObjFl_Fall_Block_cD1Ev, 0x02112380, size 0x50 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN20daObjFl_Fall_Block_cD1Ev
/* The compiler emits D2, D0 and D1 from this one class definition. */
