//cpp
/* Manually curated shadow translation unit.
 * ov047/daObjKm3_Kaitendai_c  (4 function(s))
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
 *   [0] 0x021112bc  src/_ZN20daObjKm3_Kaitendai_cD1Ev.cpp
 *   [1] 0x0211130c  src/_ZN20daObjKm3_Kaitendai_cD0Ev.c
 *   [2] 0x02111370  src/_ZN20daObjKm3_Kaitendai_c16CleanupResourcesEv.cpp
 *   [3] 0x02111384  src/_ZN20daObjKm3_Kaitendai_c13InitResourcesEv.cpp
 */

#include "daObjKm3_Kaitendai_c.h"

struct ResourceDescriptor {
    void *entries[3];
};

extern "C" {
int func_ov002_020b66a8(daObjKm3_Kaitendai_c *self,
                        ResourceDescriptor *descriptor);
int func_ov002_020b676c(daObjKm3_Kaitendai_c *self,
                        ResourceDescriptor *descriptor, s16 value);
extern ResourceDescriptor data_ov047_02112334;
extern s16 data_ov047_02112320;
extern s16 data_ov047_02112324;
}

/* ROM ordinal 3 -- _ZN20daObjKm3_Kaitendai_c13InitResourcesEv */
// @symbol _ZN20daObjKm3_Kaitendai_c13InitResourcesEv
/* Selects one of the two spawn values, then delegates setup with this class's
 * three-word resource descriptor. */
int daObjKm3_Kaitendai_c::InitResources()
{
    s16 value = data_ov047_02112320;

    if ((param1 & 0xff) == 1) {
        value = data_ov047_02112324;
    }

    return func_ov002_020b676c(this, &data_ov047_02112334, value);
}

/* ROM ordinal 2 -- _ZN20daObjKm3_Kaitendai_c16CleanupResourcesEv */
// @symbol _ZN20daObjKm3_Kaitendai_c16CleanupResourcesEv
int daObjKm3_Kaitendai_c::CleanupResources()
{
    return func_ov002_020b66a8(this, &data_ov047_02112334);
}

/* ROM ordinal 0 -- _ZN20daObjKm3_Kaitendai_cD1Ev */
// @symbol _ZN20daObjKm3_Kaitendai_cD1Ev
/* The compiler emits D2, D0 and D1 from this one class definition. */
daObjKm3_Kaitendai_c::~daObjKm3_Kaitendai_c()
{
}
