/* Sprite template list, 4 OamAttrs. */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data.gap4" ".data.gap4" ".data.gap4" far_data
#pragma section obj begin
struct OamAttr data_ov006_02133e1c[4] = {
    { 0x00f5, 0x01f6, 0x5185, 0x0000 },
    { 0x00f5, 0x0003, 0x5186, 0x0000 },
    { 0x0002, 0x0003, 0x51a7, 0x0000 },
    { 0x0002, 0x31f6, 0x5186, 0xffff },
};
#pragma section obj end
