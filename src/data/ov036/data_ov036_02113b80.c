/* ov036 cartridge data at 0x02113b80, 16 bytes. */
#include "types.h"

extern void _ZN16daObjKaitendai_c8BehaviorEv(void);
extern void _ZN19daObjRc_Kaitendai_c16CleanupResourcesEv(void);
extern void _ZN8dActor_c21AfterCleanupResourcesEj(void);
extern void _ZN8dActor_c22BeforeCleanupResourcesEv(void);

u32 data_ov036_02113b80[4] = {
    (u32)&_ZN19daObjRc_Kaitendai_c16CleanupResourcesEv, (u32)&_ZN8dActor_c22BeforeCleanupResourcesEv, (u32)&_ZN8dActor_c21AfterCleanupResourcesEj, (u32)&_ZN16daObjKaitendai_c8BehaviorEv,
};
