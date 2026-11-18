/* ov009 cartridge data at 0x02113a50, 36 bytes. */
#include "types.h"

extern void _ZN14daObjMcWater_cD0Ev(void);
extern void _ZN14daObjMcWater_cD1Ev(void);
extern void _ZN7fBase_c13OnHeapCreatedEv(void);
extern void _ZN7fBase_c9Virtual38Ejj(void);
extern void _ZN8dActor_c11OnAttacked1ERS_(void);
extern void _ZN8dActor_c13OnTurnIntoEggER7daPly_c(void);
extern void _ZN8dActor_c13OnYoshiTryEatEv(void);
extern void _ZN8dActor_c15OnGroundPoundedERS_(void);
extern void _ZN8dActor_c9Virtual50Ev(void);

u32 data_ov009_02113a50[9] = {
    (u32)&_ZN7fBase_c9Virtual38Ejj, (u32)&_ZN7fBase_c13OnHeapCreatedEv, (u32)&_ZN14daObjMcWater_cD1Ev, (u32)&_ZN14daObjMcWater_cD0Ev, (u32)&_ZN8dActor_c13OnYoshiTryEatEv, (u32)&_ZN8dActor_c13OnTurnIntoEggER7daPly_c,
    (u32)&_ZN8dActor_c9Virtual50Ev, (u32)&_ZN8dActor_c15OnGroundPoundedERS_, (u32)&_ZN8dActor_c11OnAttacked1ERS_,
};
