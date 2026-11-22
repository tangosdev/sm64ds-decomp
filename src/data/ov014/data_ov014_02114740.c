/* claimed-storage */
/* ov014 .data.gap2 0x02114740..0x0211476c. 3 storage addresses, one object. */
#include "types.h"

extern void _Z20daWanwan_c_classInitv(void);

#pragma define_section claimsec ".data.gap2" ".data.gap2" ".data.gap2" far_data
#pragma section claimsec begin
#pragma explicit_zero_data on
u32 _ZTS10daWanwan_c[] = {
    0x61643031, 0x776e6157, 0x635f6e61, 0x00000000,
    (u32)&_Z20daWanwan_c_classInitv,
    0x004300db, 0x10000003, 0x00120000, 0x00400000, 0x01000000, 0x01000000,
};
#pragma explicit_zero_data off
#pragma section claimsec end
