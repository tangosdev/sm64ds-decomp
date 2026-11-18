/* ov032 cartridge data at 0x02113a00, 64 bytes. */
#include "types.h"

extern void _ZN10dBgActor_c4KillEv(void);
extern void _ZN8dActor_c11OnAttacked2ERS_(void);
extern void _ZN8dActor_c15OnHitByMegaCharER7daPly_c(void);
extern void _ZN8dActor_c16OnAimedAtWithEggEv(void);
extern void _ZN8dActor_c19OnHitFromUnderneathERS_(void);
extern void _ZN8dActor_c24OnHitByCannonBlastedCharERS_(void);
extern void _ZN8dActor_c25OnAimedAtWithEggReturnVecEv(void);
extern void _ZN8dActor_c8OnKickedERS_(void);
extern void _ZN8dActor_c8OnPushedERS_(void);

u32 data_ov032_02113a00[16] = {
    (u32)&_ZN8dActor_c11OnAttacked2ERS_, (u32)&_ZN8dActor_c8OnKickedERS_, (u32)&_ZN8dActor_c8OnPushedERS_, (u32)&_ZN8dActor_c24OnHitByCannonBlastedCharERS_, (u32)&_ZN8dActor_c15OnHitByMegaCharER7daPly_c, (u32)&_ZN8dActor_c19OnHitFromUnderneathERS_,
    (u32)&_ZN8dActor_c16OnAimedAtWithEggEv, (u32)&_ZN8dActor_c25OnAimedAtWithEggReturnVecEv, (u32)&_ZN10dBgActor_c4KillEv, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};
