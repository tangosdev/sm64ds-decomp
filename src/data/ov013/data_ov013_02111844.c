/* ov013 cartridge data at 0x02111844, 28 bytes. */
#include "types.h"

extern u32 data_ov013_021117a4[];
extern u32 data_ov013_02111918[];
extern u32 data_ov013_02111b94[];

u32 data_ov013_02111844[7] = {
    (u32)&data_ov013_02111b94,
    (u32)&data_ov013_021117a4,
    0x06f306fa,
    0x06fc06fb,
    (u32)&data_ov013_02111918,
    0x06400007,
    0x0000000f,
};
