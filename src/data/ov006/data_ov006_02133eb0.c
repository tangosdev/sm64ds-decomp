/* Sprite template list, 4 OamAttrs. */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data.gap4" ".data.gap4" ".data.gap4" far_data
#pragma section obj begin
struct OamAttr data_ov006_02133eb0[4] = {
    { 0x00f0, 0x01f0, 0x5185, 0x0000 },
    { 0x00f0, 0x0000, 0x5166, 0x0000 },
    { 0x00f0, 0x0008, 0x5186, 0x0000 },
    { 0x0008, 0x0008, 0x51a7, 0xffff },
};
#pragma section obj end
