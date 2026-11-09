#ifndef NITRO_OS_PROTECTIONREGION_H
#define NITRO_OS_PROTECTIONREGION_H

/*
 * The protection region setters of the Nitro SDK's os_protectionRegion.c,
 * under the SDK's own names (pret/pokediamond
 * arm9/lib/NitroSDK/include/OS_protectionRegion.h). The body is in
 * src/engine/cp15/CP15_MPUDataRegion1.cpp; it writes CP15 c6,c1, and the value
 * packs the region's base, size and enable bit, as init_cp15
 * (src/init_cp15.c) shows when it programs all eight regions. Of the SDK's
 * setters only region 1's is linked into this ROM.
 *
 * The parameters are unnamed on purpose: a named parameter in a shared
 * declaration costs every including C++ file an @NNN number.
 */

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

void OS_SetProtectionRegion1(u32);

#ifdef __cplusplus
}
#endif

#endif /* NITRO_OS_PROTECTIONREGION_H */
