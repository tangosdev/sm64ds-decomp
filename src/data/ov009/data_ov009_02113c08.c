/* ov009 cartridge data at 0x02113c08, 12 bytes. */
#include "types.h"

extern void _ZN8dActor_c15OnHitByMegaCharER7daPly_c(void);
extern void _ZN8dActor_c19OnHitFromUnderneathERS_(void);
extern void _ZN8dActor_c24OnHitByCannonBlastedCharERS_(void);

u32 data_ov009_02113c08[3] = {
    (u32)&_ZN8dActor_c24OnHitByCannonBlastedCharERS_, (u32)&_ZN8dActor_c15OnHitByMegaCharER7daPly_c, (u32)&_ZN8dActor_c19OnHitFromUnderneathERS_,
};
