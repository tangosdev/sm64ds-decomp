/* Sprite template list, 6 OamAttrs. */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data.gap4" ".data.gap4" ".data.gap4" far_data
#pragma section obj begin
struct OamAttr data_ov006_02133ed0[6] = {
    { 0x80f0, 0x01f0, 0x5144, 0x0000 },
    { 0x8000, 0x01f0, 0x5184, 0x0000 },
    { 0x00f0, 0x01f8, 0x5145, 0x0000 },
    { 0x40f0, 0x0000, 0x5146, 0x0000 },
    { 0x00f8, 0x0008, 0x5165, 0x0000 },
    { 0x4008, 0x0000, 0x51a5, 0xffff },
};
#pragma section obj end
