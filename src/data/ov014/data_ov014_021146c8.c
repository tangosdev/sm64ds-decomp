/* ov014 cartridge data at 0x021146c8, 8 bytes. */
#include "types.h"

extern void _ZN10daWanwan_c19StateBombDamageMainEv(void);

#pragma define_section data_ov014_021146c8 ".data.gap_pmf" ".data.gap_pmf" ".data.gap_pmf" far_data
#pragma section data_ov014_021146c8 begin
u32 data_ov014_021146c8[2] = {
    (u32)&_ZN10daWanwan_c19StateBombDamageMainEv, 0x00000000,
};
#pragma section data_ov014_021146c8 end
