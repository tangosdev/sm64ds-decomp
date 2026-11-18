/* ov014 cartridge data at 0x021146d8, 8 bytes. */
#include "types.h"

extern void _ZN10daWanwan_c19StateBombDamageInitEv(void);

#pragma define_section data_ov014_021146d8 ".data.gap_pmf" ".data.gap_pmf" ".data.gap_pmf" far_data
#pragma section data_ov014_021146d8 begin
u32 data_ov014_021146d8[2] = {
    (u32)&_ZN10daWanwan_c19StateBombDamageInitEv, 0x00000000,
};
#pragma section data_ov014_021146d8 end
