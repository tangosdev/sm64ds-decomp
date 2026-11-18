/* ov029 cartridge data at 0x021140f8, 16 bytes. */
#include "types.h"

extern void _ZN14daObjWc_Mizu_c6RenderEv(void);
extern void _ZN8dActor_c12BeforeRenderEv(void);
extern void _ZN8dActor_c13AfterBehaviorEj(void);
extern void _ZN8dActor_c14BeforeBehaviorEv(void);

u32 data_ov029_021140f8[4] = {
    (u32)&_ZN8dActor_c14BeforeBehaviorEv, (u32)&_ZN8dActor_c13AfterBehaviorEj, (u32)&_ZN14daObjWc_Mizu_c6RenderEv, (u32)&_ZN8dActor_c12BeforeRenderEv,
};
