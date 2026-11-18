/* ov043 cartridge data at 0x021124c4, 16 bytes. */
#include "types.h"

extern void _ZN8dActor_c15OnHitByMegaCharER7daPly_c(void);
extern void _ZN8dActor_c16OnAimedAtWithEggEv(void);
extern void _ZN8dActor_c19OnHitFromUnderneathERS_(void);
extern void _ZN8dActor_c24OnHitByCannonBlastedCharERS_(void);

u32 data_ov043_021124c4[4] = {
    (u32)&_ZN8dActor_c24OnHitByCannonBlastedCharERS_, (u32)&_ZN8dActor_c15OnHitByMegaCharER7daPly_c, (u32)&_ZN8dActor_c19OnHitFromUnderneathERS_, (u32)&_ZN8dActor_c16OnAimedAtWithEggEv,
};
