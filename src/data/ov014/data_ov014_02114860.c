/* ov014 cartridge data at 0x02114860, 8 bytes. */
#include "types.h"

extern void _ZN8dActor_c16OnAimedAtWithEggEv(void);
extern void _ZN8dActor_c25OnAimedAtWithEggReturnVecEv(void);

#pragma define_section data_ov014_02114860 ".data.gap3" ".data.gap3" ".data.gap3" far_data
#pragma section data_ov014_02114860 begin
u32 data_ov014_02114860[2] = {
    (u32)&_ZN8dActor_c16OnAimedAtWithEggEv, (u32)&_ZN8dActor_c25OnAimedAtWithEggReturnVecEv,
};
#pragma section data_ov014_02114860 end
