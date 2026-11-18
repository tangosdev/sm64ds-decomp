/* ov029 cartridge data at 0x02113f4c, 44 bytes. */
#include "types.h"

extern void _ZN17daObjFloatBoard_c16CleanupResourcesEv(void);
extern void _ZN17daObjFloatBoard_c6RenderEv(void);
extern void _ZN17daObjFloatBoard_c8BehaviorEv(void);
extern void _ZN7fBase_c16OnPendingDestroyEv(void);
extern void _ZN8dActor_c11AfterRenderEj(void);
extern void _ZN8dActor_c12BeforeRenderEv(void);
extern void _ZN8dActor_c13AfterBehaviorEj(void);
extern void _ZN8dActor_c14BeforeBehaviorEv(void);
extern void _ZN8dActor_c18AfterInitResourcesEj(void);
extern void _ZN8dActor_c21AfterCleanupResourcesEj(void);
extern void _ZN8dActor_c22BeforeCleanupResourcesEv(void);

u32 data_ov029_02113f4c[11] = {
    (u32)&_ZN8dActor_c18AfterInitResourcesEj, (u32)&_ZN17daObjFloatBoard_c16CleanupResourcesEv, (u32)&_ZN8dActor_c22BeforeCleanupResourcesEv, (u32)&_ZN8dActor_c21AfterCleanupResourcesEj, (u32)&_ZN17daObjFloatBoard_c8BehaviorEv, (u32)&_ZN8dActor_c14BeforeBehaviorEv,
    (u32)&_ZN8dActor_c13AfterBehaviorEj, (u32)&_ZN17daObjFloatBoard_c6RenderEv, (u32)&_ZN8dActor_c12BeforeRenderEv, (u32)&_ZN8dActor_c11AfterRenderEj, (u32)&_ZN7fBase_c16OnPendingDestroyEv,
};
