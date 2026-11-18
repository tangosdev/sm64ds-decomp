/* ov032 cartridge data at 0x021138f0, 12 bytes. */
#include "types.h"

extern void _ZN13daObjTdFuta_c8BehaviorEv(void);
extern void _ZN8dActor_c21AfterCleanupResourcesEj(void);
extern void _ZN8dActor_c22BeforeCleanupResourcesEv(void);

u32 data_ov032_021138f0[3] = {
    (u32)&_ZN8dActor_c22BeforeCleanupResourcesEv, (u32)&_ZN8dActor_c21AfterCleanupResourcesEj, (u32)&_ZN13daObjTdFuta_c8BehaviorEv,
};
