/* ov029 cartridge data at 0x02112f88, 24 bytes. */
#include "types.h"

extern u32 data_ov029_02112ed8[];
extern u32 data_ov029_021133d4[];
extern u32 data_ov029_021134ec[];

#pragma define_section data_ov029_02112f88 ".data.w12ea0"                                          \
                                           ".data.w12ea0"                                          \
                                           ".data.w12ea0" far_data
#pragma section data_ov029_02112f88 begin
u32 data_ov029_02112f88[6] = {
    0x00001e00, (u32)&data_ov029_021134ec, 0x00002305, (u32)&data_ov029_021133d4,
    0x000001a0, (u32)&data_ov029_02112ed8,
};
#pragma section data_ov029_02112f88 end
