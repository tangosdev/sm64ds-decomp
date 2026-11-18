/* ov009 cartridge data at 0x02113998, 24 bytes. */
#include "types.h"

extern void _ZN8dActor_c13OnTurnIntoEggER7daPly_c(void);
extern void _ZN8dActor_c13OnYoshiTryEatEv(void);
extern void _ZN8dActor_c15OnGroundPoundedERS_(void);
extern void _ZN8dActor_c9Virtual50Ev(void);
extern void _ZN9daSBird_cD0Ev(void);
extern void _ZN9daSBird_cD1Ev(void);

u32 data_ov009_02113998[6] = {
    (u32)&_ZN9daSBird_cD1Ev, (u32)&_ZN9daSBird_cD0Ev, (u32)&_ZN8dActor_c13OnYoshiTryEatEv, (u32)&_ZN8dActor_c13OnTurnIntoEggER7daPly_c, (u32)&_ZN8dActor_c9Virtual50Ev, (u32)&_ZN8dActor_c15OnGroundPoundedERS_,
};
