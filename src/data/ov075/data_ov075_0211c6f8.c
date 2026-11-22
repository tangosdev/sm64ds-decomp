/* claimed-storage */
/* ov075 .data.entry_sprite_gap0 0x0211c6f8..0x0211c708. 2 storage addresses, one object. */
#include "types.h"

extern void _ZN10dScEntry_c16EnterSoloConfirmEv(void);
extern void _ZN10dScEntry_c16EnterStepFadeOutEv(void);

#pragma define_section claimsec ".data.entry_sprite_gap0" ".data.entry_sprite_gap0" ".data.entry_sprite_gap0" far_data
#pragma section claimsec begin
u32 data_ov075_0211c6f8[] = {
    (u32)&_ZN10dScEntry_c16EnterSoloConfirmEv,
    0x00000000,
    (u32)&_ZN10dScEntry_c16EnterStepFadeOutEv,
    0x00000000,
};
#pragma section claimsec end
