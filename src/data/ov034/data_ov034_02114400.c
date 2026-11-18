/* ov034 cartridge data at 0x02114400, 8 bytes. */
#include "types.h"

extern void _ZN12daHanachan_c15StateDamageMainEv(void);

u32 data_ov034_02114400[2] = {
    (u32)&_ZN12daHanachan_c15StateDamageMainEv, 0x00000000,
};
