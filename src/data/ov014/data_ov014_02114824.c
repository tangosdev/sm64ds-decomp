/* ov014 cartridge data at 0x02114824, 60 bytes. */
#include "types.h"

extern void _ZN10daWanwan_cD0Ev(void);
extern void _ZN10daWanwan_cD1Ev(void);
extern void _ZN7fBase_c13OnHeapCreatedEv(void);
extern void _ZN7fBase_c9Virtual38Ejj(void);
extern void _ZN8dActor_c11OnAttacked1ERS_(void);
extern void _ZN8dActor_c11OnAttacked2ERS_(void);
extern void _ZN8dActor_c13OnTurnIntoEggER7daPly_c(void);
extern void _ZN8dActor_c13OnYoshiTryEatEv(void);
extern void _ZN8dActor_c15OnGroundPoundedERS_(void);
extern void _ZN8dActor_c15OnHitByMegaCharER7daPly_c(void);
extern void _ZN8dActor_c19OnHitFromUnderneathERS_(void);
extern void _ZN8dActor_c24OnHitByCannonBlastedCharERS_(void);
extern void _ZN8dActor_c8OnKickedERS_(void);
extern void _ZN8dActor_c8OnPushedERS_(void);
extern void _ZN8dActor_c9Virtual50Ev(void);

#pragma define_section data_ov014_02114824 ".data.gap3" ".data.gap3" ".data.gap3" far_data
#pragma section data_ov014_02114824 begin
u32 data_ov014_02114824[15] = {
    (u32)&_ZN7fBase_c9Virtual38Ejj, (u32)&_ZN7fBase_c13OnHeapCreatedEv, (u32)&_ZN10daWanwan_cD1Ev, (u32)&_ZN10daWanwan_cD0Ev, (u32)&_ZN8dActor_c13OnYoshiTryEatEv, (u32)&_ZN8dActor_c13OnTurnIntoEggER7daPly_c,
    (u32)&_ZN8dActor_c9Virtual50Ev, (u32)&_ZN8dActor_c15OnGroundPoundedERS_, (u32)&_ZN8dActor_c11OnAttacked1ERS_, (u32)&_ZN8dActor_c11OnAttacked2ERS_, (u32)&_ZN8dActor_c8OnKickedERS_, (u32)&_ZN8dActor_c8OnPushedERS_,
    (u32)&_ZN8dActor_c24OnHitByCannonBlastedCharERS_, (u32)&_ZN8dActor_c15OnHitByMegaCharER7daPly_c, (u32)&_ZN8dActor_c19OnHitFromUnderneathERS_,
};
#pragma section data_ov014_02114824 end
