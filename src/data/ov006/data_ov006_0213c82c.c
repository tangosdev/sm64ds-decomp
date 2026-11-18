/*
 * One object for data_ov006_0213c82c, sEsp3DTextureAnim (48 bytes): the
 * linker starts every input section on a 4-byte boundary, so these tables
 * cannot be separate.
 */
#include "types.h"

#pragma define_section obj ".data" ".data" ".data" far_data
#pragma section obj begin
extern u32 data_ov006_0212e62c[];
extern u32 data_ov006_0212e5a8[];
extern u8 data_ov006_0212e584[];
extern u32 data_ov006_0212e580[];
u32 data_ov006_0213c82c[12] = {
    0x00400040, 0x00400080, 0x004000c0, 0x00800040, 0x00800080, 0x008000c0, 0x00000078, (u32)&data_ov006_0212e584,
    (u32)&data_ov006_0212e580, (u32)&data_ov006_0212e62c, 0x00000001, (u32)&data_ov006_0212e5a8,
};
#pragma section obj end
