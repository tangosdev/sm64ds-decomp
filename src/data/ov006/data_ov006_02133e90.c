/* Sprite template list, 4 OamAttrs. */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data.gap4" ".data.gap4" ".data.gap4" far_data
#pragma section obj begin
struct OamAttr data_ov006_02133e90[4] = {
    { 0x00f0, 0x41f0, 0x5142, 0x0000 },
    { 0x0000, 0x61f0, 0x5142, 0x0000 },
    { 0x00f0, 0x5000, 0x5142, 0x0000 },
    { 0x0000, 0x7000, 0x5142, 0xffff },
};
#pragma section obj end
