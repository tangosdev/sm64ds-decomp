/* ov013 cartridge data at 0x02112178, 32 bytes. */
#include "types.h"

extern void _ZN8dActor_c11OnAttacked1ERS_(void);
extern void _ZN8dActor_c11OnAttacked2ERS_(void);
extern void _ZN8dActor_c15OnGroundPoundedERS_(void);
extern void _ZN8dActor_c15OnHitByMegaCharER7daPly_c(void);
extern void _ZN8dActor_c24OnHitByCannonBlastedCharERS_(void);
extern void _ZN8dActor_c8OnKickedERS_(void);
extern void _ZN8dActor_c8OnPushedERS_(void);
extern void _ZN8dActor_c9Virtual50Ev(void);

u32 data_ov013_02112178[8] = {
    (u32)&_ZN8dActor_c9Virtual50Ev, (u32)&_ZN8dActor_c15OnGroundPoundedERS_, (u32)&_ZN8dActor_c11OnAttacked1ERS_, (u32)&_ZN8dActor_c11OnAttacked2ERS_, (u32)&_ZN8dActor_c8OnKickedERS_, (u32)&_ZN8dActor_c8OnPushedERS_,
    (u32)&_ZN8dActor_c24OnHitByCannonBlastedCharERS_, (u32)&_ZN8dActor_c15OnHitByMegaCharER7daPly_c,
};
