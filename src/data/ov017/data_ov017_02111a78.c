/* claimed-storage */
/* ov017 .data.ksMid 0x02111a78..0x02111a94. 1 storage addresses, one object. */
#include "types.h"

extern u32 data_ov017_02111ab4[];
extern u32 data_ov017_02111a08[];
extern u32 data_ov017_02111a24[];

#pragma define_section claimsec ".data.ksMid" ".data.ksMid" ".data.ksMid" far_data
#pragma section claimsec begin
#pragma explicit_zero_data on
u32 data_ov017_02111a78[] = {
    (u32)&data_ov017_02111ab4,
    (u32)&data_ov017_02111a08,
    0x07640766, 0x07680767,
    (u32)&data_ov017_02111a24,
    0x03200001, 0x0000000f,
};
#pragma explicit_zero_data off
#pragma section claimsec end
