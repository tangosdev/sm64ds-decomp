/* ov013 cartridge data at 0x02112158, 32 bytes. */
#include "types.h"

extern void _ZN18daObjClockHuriko_cD0Ev(void);
extern void _ZN18daObjClockHuriko_cD1Ev(void);
extern void _ZN7fBase_c13OnHeapCreatedEv(void);
extern void _ZN7fBase_c16OnPendingDestroyEv(void);
extern void _ZN7fBase_c9Virtual34Ejj(void);
extern void _ZN7fBase_c9Virtual38Ejj(void);
extern void _ZN8dActor_c13OnTurnIntoEggER7daPly_c(void);
extern void _ZN8dActor_c13OnYoshiTryEatEv(void);

u32 data_ov013_02112158[8] = {
    (u32)&_ZN7fBase_c16OnPendingDestroyEv, (u32)&_ZN7fBase_c9Virtual34Ejj, (u32)&_ZN7fBase_c9Virtual38Ejj, (u32)&_ZN7fBase_c13OnHeapCreatedEv, (u32)&_ZN18daObjClockHuriko_cD1Ev, (u32)&_ZN18daObjClockHuriko_cD0Ev,
    (u32)&_ZN8dActor_c13OnYoshiTryEatEv, (u32)&_ZN8dActor_c13OnTurnIntoEggER7daPly_c,
};
