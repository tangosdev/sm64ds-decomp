/* ov009 cartridge data at 0x02113980, 24 bytes. */
#include "types.h"

extern void _ZN7fBase_c13OnHeapCreatedEv(void);
extern void _ZN7fBase_c9Virtual34Ejj(void);
extern void _ZN7fBase_c9Virtual38Ejj(void);
extern void _ZN8dActor_c11AfterRenderEj(void);
extern void _ZN8dActor_c12BeforeRenderEv(void);
extern void _ZN9daSBird_c16OnPendingDestroyEv(void);

u32 data_ov009_02113980[6] = {
    (u32)&_ZN8dActor_c12BeforeRenderEv, (u32)&_ZN8dActor_c11AfterRenderEj, (u32)&_ZN9daSBird_c16OnPendingDestroyEv, (u32)&_ZN7fBase_c9Virtual34Ejj, (u32)&_ZN7fBase_c9Virtual38Ejj, (u32)&_ZN7fBase_c13OnHeapCreatedEv,
};
