/* ov014 cartridge data at 0x021145c4, 12 bytes. */
#include "types.h"

extern u32 data_ov014_02114578[];
extern u32 gBAnaShutterCollisionFile[];
extern u32 gBAnaShutterModelFile[];

#pragma define_section data_ov014_021145c4 ".data.gap0" ".data.gap0" ".data.gap0" far_data
#pragma section data_ov014_021145c4 begin
u32 data_ov014_021145c4[3] = {
    (u32)&gBAnaShutterModelFile, (u32)&gBAnaShutterCollisionFile, (u32)&data_ov014_02114578,
};
#pragma section data_ov014_021145c4 end
