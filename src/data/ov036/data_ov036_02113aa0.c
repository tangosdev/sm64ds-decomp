/* ov036 cartridge data at 0x02113aa0, 28 bytes. */
#include "types.h"

extern void _ZN16daObjRcBuranko_c16CleanupResourcesEv(void);
extern void _ZN16daObjRcBuranko_c8BehaviorEv(void);
extern void _ZN8dActor_c13AfterBehaviorEj(void);
extern void _ZN8dActor_c14BeforeBehaviorEv(void);
extern void _ZN8dActor_c18AfterInitResourcesEj(void);
extern void _ZN8dActor_c21AfterCleanupResourcesEj(void);
extern void _ZN8dActor_c22BeforeCleanupResourcesEv(void);

u32 data_ov036_02113aa0[7] = {
    (u32)&_ZN8dActor_c18AfterInitResourcesEj, (u32)&_ZN16daObjRcBuranko_c16CleanupResourcesEv, (u32)&_ZN8dActor_c22BeforeCleanupResourcesEv, (u32)&_ZN8dActor_c21AfterCleanupResourcesEj, (u32)&_ZN16daObjRcBuranko_c8BehaviorEv, (u32)&_ZN8dActor_c14BeforeBehaviorEv,
    (u32)&_ZN8dActor_c13AfterBehaviorEj,
};
