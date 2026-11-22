/* claimed-storage */
/* ov075 .data.entry_sprite_gap1 0x0211c710..0x0211c720. 2 storage addresses, one object. */
#include "types.h"

extern void _ZN10dScEntry_c17RenderWaitingTextEv(void);

#pragma define_section claimsec ".data.entry_sprite_gap1" ".data.entry_sprite_gap1" ".data.entry_sprite_gap1" far_data
#pragma section claimsec begin
u32 data_ov075_0211c710[] = {
    0x001c001f, 0xffffe11f,
    (u32)&_ZN10dScEntry_c17RenderWaitingTextEv,
    0x00000000,
};
#pragma section claimsec end
