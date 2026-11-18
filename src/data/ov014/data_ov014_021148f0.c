/* ov014 cartridge data at 0x021148f0, 80 bytes. */
#include "types.h"

extern void _ZN10dBgActor_c4KillEv(void);
extern void _ZN20daObjWanwanShutter_cD0Ev(void);
extern void _ZN20daObjWanwanShutter_cD1Ev(void);
extern void _ZN8dActor_c11OnAttacked1ERS_(void);
extern void _ZN8dActor_c11OnAttacked2ERS_(void);
extern void _ZN8dActor_c13OnTurnIntoEggER7daPly_c(void);
extern void _ZN8dActor_c13OnYoshiTryEatEv(void);
extern void _ZN8dActor_c15OnGroundPoundedERS_(void);
extern void _ZN8dActor_c15OnHitByMegaCharER7daPly_c(void);
extern void _ZN8dActor_c16OnAimedAtWithEggEv(void);
extern void _ZN8dActor_c19OnHitFromUnderneathERS_(void);
extern void _ZN8dActor_c24OnHitByCannonBlastedCharERS_(void);
extern void _ZN8dActor_c25OnAimedAtWithEggReturnVecEv(void);
extern void _ZN8dActor_c8OnKickedERS_(void);
extern void _ZN8dActor_c8OnPushedERS_(void);
extern void _ZN8dActor_c9Virtual50Ev(void);

#pragma define_section data_ov014_021148f0 ".data.gap3" ".data.gap3" ".data.gap3" far_data
#pragma section data_ov014_021148f0 begin
u32 data_ov014_021148f0[20] = {
    (u32)&_ZN20daObjWanwanShutter_cD1Ev, (u32)&_ZN20daObjWanwanShutter_cD0Ev, (u32)&_ZN8dActor_c13OnYoshiTryEatEv, (u32)&_ZN8dActor_c13OnTurnIntoEggER7daPly_c, (u32)&_ZN8dActor_c9Virtual50Ev, (u32)&_ZN8dActor_c15OnGroundPoundedERS_,
    (u32)&_ZN8dActor_c11OnAttacked1ERS_, (u32)&_ZN8dActor_c11OnAttacked2ERS_, (u32)&_ZN8dActor_c8OnKickedERS_, (u32)&_ZN8dActor_c8OnPushedERS_, (u32)&_ZN8dActor_c24OnHitByCannonBlastedCharERS_, (u32)&_ZN8dActor_c15OnHitByMegaCharER7daPly_c,
    (u32)&_ZN8dActor_c19OnHitFromUnderneathERS_, (u32)&_ZN8dActor_c16OnAimedAtWithEggEv, (u32)&_ZN8dActor_c25OnAimedAtWithEggReturnVecEv, (u32)&_ZN10dBgActor_c4KillEv, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};
#pragma section data_ov014_021148f0 end
