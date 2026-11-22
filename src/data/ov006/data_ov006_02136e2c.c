/* claimed-storage */
/* ov006 .data.gap23 0x02136e2c..0x02136e38. 2 storage addresses, one object. */
#include "types.h"

extern u32 data_ov006_02136e30[];

#pragma define_section claimsec ".data.gap23" ".data.gap23" ".data.gap23" far_data
#pragma section claimsec begin
#pragma explicit_zero_data on
u32 sPanelBannerSprites[] = {
    (u32)&data_ov006_02136e30,
    0x41f800f8, 0xffff0088,
};
#pragma explicit_zero_data off
#pragma section claimsec end
