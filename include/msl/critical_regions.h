#ifndef MSL_CRITICAL_REGIONS_H
#define MSL_CRITICAL_REGIONS_H

/* MSL's critical regions on NITRO: one NitroSDK mutex per region, with the
 * owning thread's id and a recursion count beside it. MSL inlines
 * __begin_critical_region and __end_critical_region into each caller. The
 * ROM lays the three arrays out as pret pokediamond's
 * arm9/asm/MSL_ARM_critical_regions_NITRO.s does -- __cs_id, __cs_ref, then
 * __cs -- at 0x020a9fd8, 0x020a9ffc and 0x020aa020, nine regions each.
 * __exit takes region 0 and raise region 7 (+0x1c in the id and count
 * arrays, +0x8c in __cs, whose OSMutex is 0x14 bytes here). */

#include "nitro/os/thread.h"

enum critical_regions {
    atexit_funcs_access = 0,
    malloc_pool_access = 1,
    stdin_access = 2,
    stdout_access = 3,
    stderr_access = 4,
    files_access = 5,
    console_status_access = 6,
    signal_funcs_access = 7,
    thread_access = 8,
    num_critical_regions = 9
};

#ifdef __cplusplus
#ifdef _MSC_VER
extern "C" {
#endif
#endif

extern int __cs_id[num_critical_regions];
extern int __cs_ref[num_critical_regions];
extern OSMutex __cs[num_critical_regions];

#ifdef __cplusplus
#ifdef _MSC_VER
}
#endif
#endif

#endif /* MSL_CRITICAL_REGIONS_H */
