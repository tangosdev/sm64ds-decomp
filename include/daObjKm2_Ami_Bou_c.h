#ifndef DAOBJKM2_AMI_BOU_C_H
#define DAOBJKM2_AMI_BOU_C_H

#include "types.h"
#include "dBgActor_c.h"
#include "dCcAc_c.h"

/* daKpa_c in the Fire Sea net pole (profile KM2_AMI_BOU).
 * Direct base dBgActor_c. Factory allocates 0x358.
 * mdCcAc_c is the cylinder at 0x320. mHeightAng at 0x354 is the
 * sine-table phase: Behavior indexes this unsigned halfword, then
 * adds through a signed halfword. A plain u16 add loads ldrh where
 * the cartridge has ldrsh.
 */
struct daObjKm2_Ami_Bou_c : dBgActor_c {
    u8 pad_31e[0x2];
    dCcAc_c mdCcAc_c; /* 0x320 */
    u16 mHeightAng;   /* 0x354 */

    /* Inline is load-bearing: out-of-line emits D0 before D1. */
    virtual ~daObjKm2_Ami_Bou_c() {}

    virtual s32 InitResources();
    virtual s32 CleanupResources();
    virtual s32 Behavior();
    virtual s32 Render();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjKm2_Ami_Bou_c_size_must_be_0x358[sizeof(daObjKm2_Ami_Bou_c) == 0x358 ? 1 : -1];
#endif

#endif /* DAOBJKM2_AMI_BOU_C_H */
