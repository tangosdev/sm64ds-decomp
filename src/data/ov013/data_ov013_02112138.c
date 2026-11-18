/* ov013 cartridge data at 0x02112138, 32 bytes. */
#include "types.h"

extern void _ZN18daObjClockHuriko_c6RenderEv(void);
extern void _ZN18daObjClockHuriko_c8BehaviorEv(void);
extern void _ZN8dActor_c11AfterRenderEj(void);
extern void _ZN8dActor_c12BeforeRenderEv(void);
extern void _ZN8dActor_c13AfterBehaviorEj(void);
extern void _ZN8dActor_c14BeforeBehaviorEv(void);
extern void _ZN8dActor_c21AfterCleanupResourcesEj(void);
extern void _ZN8dActor_c22BeforeCleanupResourcesEv(void);

u32 data_ov013_02112138[8] = {
    (u32)&_ZN8dActor_c22BeforeCleanupResourcesEv, (u32)&_ZN8dActor_c21AfterCleanupResourcesEj, (u32)&_ZN18daObjClockHuriko_c8BehaviorEv, (u32)&_ZN8dActor_c14BeforeBehaviorEv, (u32)&_ZN8dActor_c13AfterBehaviorEj, (u32)&_ZN18daObjClockHuriko_c6RenderEv,
    (u32)&_ZN8dActor_c12BeforeRenderEv, (u32)&_ZN8dActor_c11AfterRenderEj,
};
