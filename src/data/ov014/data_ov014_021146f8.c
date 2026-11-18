/* ov014 cartridge data at 0x021146f8, 8 bytes. */
#include "types.h"

extern void _ZN10daWanwan_c19StateEscapeDemoInitEv(void);

#pragma define_section data_ov014_021146f8 ".data.gap_pmf" ".data.gap_pmf" ".data.gap_pmf" far_data
#pragma section data_ov014_021146f8 begin
u32 data_ov014_021146f8[2] = {
    (u32)&_ZN10daWanwan_c19StateEscapeDemoInitEv, 0x00000000,
};
#pragma section data_ov014_021146f8 end
