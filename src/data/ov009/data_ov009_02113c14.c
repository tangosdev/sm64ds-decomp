/* ov009 cartridge data at 0x02113c14, 12 bytes. */
#include "types.h"

extern void _ZN8dActor_c16OnAimedAtWithEggEv(void);
extern void _ZN8dActor_c25OnAimedAtWithEggReturnVecEv(void);

u32 data_ov009_02113c14[3] = {
    (u32)&_ZN8dActor_c16OnAimedAtWithEggEv, (u32)&_ZN8dActor_c25OnAimedAtWithEggReturnVecEv, 0x00000000,
};
