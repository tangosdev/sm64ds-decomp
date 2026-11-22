/* claimed-storage */
/* ov006 .data.gap42 0x0213ab6c..0x0213aba8. 6 storage addresses, one object. */
#include "types.h"

extern u32 data_ov006_0213ab8c[];
extern u32 data_ov006_0213ab84[];
extern u32 data_ov006_0213ab7c[];
extern u32 data_ov006_0213ab74[];

#pragma define_section claimsec ".data.gap42" ".data.gap42" ".data.gap42" far_data
#pragma section claimsec begin
#pragma explicit_zero_data on
u32 data_ov006_0213ab6c[] = {
    0xc1d800e0, 0xffff1018, 0xc1e000e0, 0xffff1118, 0xc1e000e0, 0xffff0110, 0xc1e000e0, 0xffff0108,
    0xc1e000e0, 0xffff0100,
    (u32)&data_ov006_0213ab8c,
    (u32)&data_ov006_0213ab84,
    (u32)&data_ov006_0213ab7c,
    (u32)&data_ov006_0213ab74,
    (u32)&data_ov006_0213ab6c,
};
#pragma explicit_zero_data off
#pragma section claimsec end
