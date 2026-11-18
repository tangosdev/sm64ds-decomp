/* ov043 cartridge data at 0x02112494, 24 bytes. */
#include "types.h"

extern void _ZN17daObjKm1_Kuruma_cD0Ev(void);
extern void _ZN17daObjKm1_Kuruma_cD1Ev(void);
extern void _ZN7fBase_c13OnHeapCreatedEv(void);
extern void _ZN7fBase_c9Virtual38Ejj(void);
extern void _ZN8dActor_c13OnTurnIntoEggER7daPly_c(void);
extern void _ZN8dActor_c13OnYoshiTryEatEv(void);

u32 data_ov043_02112494[6] = {
    (u32)&_ZN7fBase_c9Virtual38Ejj, (u32)&_ZN7fBase_c13OnHeapCreatedEv, (u32)&_ZN17daObjKm1_Kuruma_cD1Ev, (u32)&_ZN17daObjKm1_Kuruma_cD0Ev, (u32)&_ZN8dActor_c13OnYoshiTryEatEv, (u32)&_ZN8dActor_c13OnTurnIntoEggER7daPly_c,
};
