/* ov013 cartridge data at 0x02112198, 12 bytes. */
#include "types.h"

extern void _ZN8dActor_c16OnAimedAtWithEggEv(void);
extern void _ZN8dActor_c19OnHitFromUnderneathERS_(void);
extern void _ZN8dActor_c25OnAimedAtWithEggReturnVecEv(void);

u32 data_ov013_02112198[3] = {
    (u32)&_ZN8dActor_c19OnHitFromUnderneathERS_, (u32)&_ZN8dActor_c16OnAimedAtWithEggEv, (u32)&_ZN8dActor_c25OnAimedAtWithEggReturnVecEv,
};
