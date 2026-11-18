/* 0x02103250: menu marker word 0x98765432.
 * u32[1], exact cartridge bytes. */
#include "types.h"

#pragma define_section sec_02103250 ".data" ".data" ".data" far_data
#pragma section sec_02103250 begin
u32 data_ov007_02103250[1] = {
    0x98765432,
};
#pragma section sec_02103250 end
