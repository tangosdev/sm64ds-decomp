/* claimed-storage */
/* ov015 .data_pre 0x02113440..0x02113574. 19 storage addresses, one object. */
#include "types.h"

extern u32 data_ov015_021137d4[];
extern u32 data_ov015_021138e8[];
extern u32 data_ov015_02113450[];
extern u32 data_ov015_02113500[];
extern u32 data_ov015_02112e10[];
extern u32 data_ov015_02112e0c[];
extern u32 data_ov015_02112e30[];
extern u32 data_ov015_02112e14[];
extern u32 data_ov015_02113968[];
extern u32 data_ov015_02113448[];
extern u32 data_ov015_02113464[];

#pragma define_section claimsec ".data_pre" ".data_pre" ".data_pre" far_data
#pragma section claimsec begin
#pragma explicit_zero_data on
u32 data_ov015_02113440[] = {
    0x000006df, 0x01011114, 0x00000006,
    (u32)&data_ov015_021137d4,
    0x00000010,
    (u32)&data_ov015_021138e8,
    0x65746177, 0x616d5f72, 0x00000074,
    (u32)&data_ov015_02113450,
    (u32)&data_ov015_02113500,
    0x27100000, 0x0c08e10e, 0xf150041a, 0x0f69e30e, 0xecd70064, 0x0c08e10d, 0xf150041a, 0x0f69e30d,
    0xecd70064, 0x0c08e10e, 0xf150041a, 0x0f69e30e, 0xecd70064, 0x0c08e10e, 0xf150041a, 0x0f69e30e,
    0xecd70064, 0x0c08e10d, 0xf150041a, 0x0f69e30d, 0xecd70064, 0x09c4f4ac, 0x0000fc7c, 0x041a09c4,
    0x0040f0c4, 0x0c08e10e, 0xf150041a, 0x0f69e30e, 0xecd70064, 0x0c08e10e, 0xf150041a, 0x0f69e30e,
    0xecd70064, 0x041a09c4, 0x0011f0c4, 0x09c4f4ac, 0x1011fc7c, 0x0000005b,
    (u32)&data_ov015_02112e10,
    (u32)&data_ov015_02112e0c,
    (u32)&data_ov015_02112e30,
    0x00000001,
    (u32)&data_ov015_02112e14,
    (u32)&data_ov015_02113968,
    (u32)&data_ov015_02113448,
    0x06de06e0, 0x06e206e1,
    (u32)&data_ov015_02113464,
    0x047e0001, 0x0000002f, 0x53504c43, 0x00010008, 0x00000fc4, 0x000000ff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0x53504c43, 0x00010008, 0x00000fc4, 0x000000ff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff,
};
#pragma explicit_zero_data off
#pragma section claimsec end
