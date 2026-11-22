/* claimed-storage */
/* ov012 .data.c0Pre 0x02111b80..0x02111bc4. 10 storage addresses, one object. */
#include "types.h"

extern u32 data_ov012_02111c14[];
extern u32 data_ov012_02111ff8[];
extern u32 data_ov012_02111b8c[];
extern u32 data_ov012_02111c04[];
extern u32 data_ov012_02111d2c[];
extern u32 data_ov012_02111d74[];
extern u32 data_ov012_02111bac[];
extern u32 data_ov012_02111b84[];

#pragma define_section claimsec ".data.c0Pre" ".data.c0Pre" ".data.c0Pre" far_data
#pragma section claimsec begin
#pragma explicit_zero_data on
u32 data_ov012_02111b80[] = {
    0x00000004, 0x00000100,
    (u32)&data_ov012_02111c14,
    0x00001400,
    (u32)&data_ov012_02111ff8,
    0x00000001,
    (u32)&data_ov012_02111b8c,
    0x00000002,
    (u32)&data_ov012_02111c04,
    0x00000009,
    (u32)&data_ov012_02111d2c,
    0x00000500,
    (u32)&data_ov012_02111d74,
    0x00000001,
    (u32)&data_ov012_02111bac,
    0x00000001,
    (u32)&data_ov012_02111b84,
};
#pragma explicit_zero_data off
#pragma section claimsec end
