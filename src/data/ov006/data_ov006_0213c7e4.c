/*
 * One object for data_ov006_0213c7e4, sEsp3DMaterialAnim (32 bytes): the
 * linker starts every input section on a 4-byte boundary, so these tables
 * cannot be separate.
 */
#include "types.h"

extern u32 data_ov006_0212e5f0[];
extern u32 data_ov006_0212e5c4[];
u32 data_ov006_0213c7e4[8] = {
    0x00400060, 0x004000a0,
    0x00800060, 0x008000a0,
    0x00000028, (u32)&data_ov006_0212e5c4,
    0x00000001, (u32)&data_ov006_0212e5f0,
};
