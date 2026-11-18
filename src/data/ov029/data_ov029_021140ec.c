/* ov029 cartridge data at 0x021140ec, 12 bytes. */
#include "types.h"

extern void _ZN14daObjWc_Mizu_c8BehaviorEv(void);
extern void _ZN8dActor_c21AfterCleanupResourcesEj(void);
extern void _ZN8dActor_c22BeforeCleanupResourcesEv(void);

u32 data_ov029_021140ec[3] = {
    (u32)&_ZN8dActor_c22BeforeCleanupResourcesEv, (u32)&_ZN8dActor_c21AfterCleanupResourcesEj, (u32)&_ZN14daObjWc_Mizu_c8BehaviorEv,
};
