/* ov034 cartridge data at 0x02114380, 8 bytes. */
#include "types.h"

extern void _ZN12daHanachan_c18StateMoveStartMainEv(void);

u32 data_ov034_02114380[2] = {
    (u32)&_ZN12daHanachan_c18StateMoveStartMainEv, 0x00000000,
};
