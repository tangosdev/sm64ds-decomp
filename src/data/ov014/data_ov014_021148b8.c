/* ov014 cartridge data at 0x021148b8, 40 bytes. */
#include "types.h"

extern void _ZN20daObjWanwanShutter_c16CleanupResourcesEv(void);
extern void _ZN20daObjWanwanShutter_c6RenderEv(void);
extern void _ZN20daObjWanwanShutter_c8BehaviorEv(void);
extern void _ZN8dActor_c11AfterRenderEj(void);
extern void _ZN8dActor_c12BeforeRenderEv(void);
extern void _ZN8dActor_c13AfterBehaviorEj(void);
extern void _ZN8dActor_c14BeforeBehaviorEv(void);
extern void _ZN8dActor_c18AfterInitResourcesEj(void);
extern void _ZN8dActor_c21AfterCleanupResourcesEj(void);
extern void _ZN8dActor_c22BeforeCleanupResourcesEv(void);

#pragma define_section data_ov014_021148b8 ".data.gap3" ".data.gap3" ".data.gap3" far_data
#pragma section data_ov014_021148b8 begin
u32 data_ov014_021148b8[10] = {
    (u32)&_ZN8dActor_c18AfterInitResourcesEj, (u32)&_ZN20daObjWanwanShutter_c16CleanupResourcesEv, (u32)&_ZN8dActor_c22BeforeCleanupResourcesEv, (u32)&_ZN8dActor_c21AfterCleanupResourcesEj, (u32)&_ZN20daObjWanwanShutter_c8BehaviorEv, (u32)&_ZN8dActor_c14BeforeBehaviorEv,
    (u32)&_ZN8dActor_c13AfterBehaviorEj, (u32)&_ZN20daObjWanwanShutter_c6RenderEv, (u32)&_ZN8dActor_c12BeforeRenderEv, (u32)&_ZN8dActor_c11AfterRenderEj,
};
#pragma section data_ov014_021148b8 end
