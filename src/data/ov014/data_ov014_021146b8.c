/* ov014 cartridge data at 0x021146b8, 8 bytes. */
#include "types.h"

extern void _ZN10daWanwan_c13StateWaitMainEv(void);

#pragma define_section data_ov014_021146b8 ".data.gap_pmf" ".data.gap_pmf" ".data.gap_pmf" far_data
#pragma section data_ov014_021146b8 begin
u32 data_ov014_021146b8[2] = {
    (u32)&_ZN10daWanwan_c13StateWaitMainEv, 0x00000000,
};
#pragma section data_ov014_021146b8 end
