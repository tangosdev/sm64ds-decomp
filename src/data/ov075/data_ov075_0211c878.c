/* claimed-storage */
/* ov075 .data.entry_sprite_gap6 0x0211c878..0x0211c890. 3 storage addresses, one object. */
#include "types.h"

extern void _ZN10dScEntry_c15EnterPlayerSyncEv(void);
extern void _Z26dScEntry_c_classInit_ENTRYv(void);
extern void _ZN10dScEntry_c15RenderWinCountsEv(void);

#pragma define_section claimsec ".data.entry_sprite_gap6" ".data.entry_sprite_gap6" ".data.entry_sprite_gap6" far_data
#pragma section claimsec begin
u32 data_ov075_0211c878[] = {
    (u32)&_ZN10dScEntry_c15EnterPlayerSyncEv,
    0x00000000,
    (u32)&_Z26dScEntry_c_classInit_ENTRYv,
    0x00090006,
    (u32)&_ZN10dScEntry_c15RenderWinCountsEv,
    0x00000000,
};
#pragma section claimsec end
