/* claimed-storage */
/* ov006 .data.gap46 0x0213afd8..0x0213b008. 3 storage addresses, one object. */
#include "types.h"

extern u32 _ZTVN3abi17__class_type_infoE[];
extern u32 _ZTS22dMg3DHeyhoObjAdapter_c[];

#pragma define_section claimsec ".data.gap46" ".data.gap46" ".data.gap46" far_data
#pragma section claimsec begin
#pragma explicit_zero_data on
u32 _ZTV22dMg3DHeyhoObjAdapter_c[] = {
    0x00000000, 0x00000000, 0x00000000, 0x4d643232, 0x48443367, 0x6f687965, 0x416a624f, 0x74706164,
    0x635f7265, 0x00000000,
    (u32)&_ZTVN3abi17__class_type_infoE,
    (u32)&_ZTS22dMg3DHeyhoObjAdapter_c,
};
#pragma explicit_zero_data off
#pragma section claimsec end
