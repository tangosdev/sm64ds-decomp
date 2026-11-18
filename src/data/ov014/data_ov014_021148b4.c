/* ov014 cartridge data at 0x021148b4, 4 bytes. */
#include "types.h"

extern void _ZN8dActor_c19BeforeInitResourcesEv(void);

#pragma define_section data_ov014_021148b4 ".data.gap3" ".data.gap3" ".data.gap3" far_data
#pragma section data_ov014_021148b4 begin
u32 data_ov014_021148b4[1] = {
    (u32)&_ZN8dActor_c19BeforeInitResourcesEv,
};
#pragma section data_ov014_021148b4 end
