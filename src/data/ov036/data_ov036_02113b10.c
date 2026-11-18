/* ov036 cartridge data at 0x02113b10, 8 bytes. */
#include "types.h"

extern void _ZN10dBgActor_c4KillEv(void);
extern void _ZN8dActor_c25OnAimedAtWithEggReturnVecEv(void);

u32 data_ov036_02113b10[2] = {
    (u32)&_ZN8dActor_c25OnAimedAtWithEggReturnVecEv, (u32)&_ZN10dBgActor_c4KillEv,
};
