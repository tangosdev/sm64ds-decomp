/* ov043 cartridge data at 0x02112404, 8 bytes. */
#include "types.h"

extern void _ZN10dBgActor_c4KillEv(void);
extern void _ZN8dActor_c25OnAimedAtWithEggReturnVecEv(void);

u32 data_ov043_02112404[2] = {
    (u32)&_ZN8dActor_c25OnAimedAtWithEggReturnVecEv, (u32)&_ZN10dBgActor_c4KillEv,
};
