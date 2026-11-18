/* ov043 cartridge data at 0x02112320, 24 bytes. */
#include "types.h"

extern void _ZN10dBgActor_c4KillEv(void);
extern void _ZN8dActor_c15OnHitByMegaCharER7daPly_c(void);
extern void _ZN8dActor_c16OnAimedAtWithEggEv(void);
extern void _ZN8dActor_c19OnHitFromUnderneathERS_(void);
extern void _ZN8dActor_c24OnHitByCannonBlastedCharERS_(void);
extern void _ZN8dActor_c25OnAimedAtWithEggReturnVecEv(void);

u32 data_ov043_02112320[6] = {
    (u32)&_ZN8dActor_c24OnHitByCannonBlastedCharERS_, (u32)&_ZN8dActor_c15OnHitByMegaCharER7daPly_c, (u32)&_ZN8dActor_c19OnHitFromUnderneathERS_, (u32)&_ZN8dActor_c16OnAimedAtWithEggEv, (u32)&_ZN8dActor_c25OnAimedAtWithEggReturnVecEv, (u32)&_ZN10dBgActor_c4KillEv,
};
