/* Sprite template list, 6 OamAttrs. */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data.gap4" ".data.gap4" ".data.gap4" far_data
#pragma section obj begin
struct OamAttr data_ov006_02133e3c[6] = {
    { 0x80f6, 0x01f7, 0x5144, 0x0000 },
    { 0x00f6, 0x11f8, 0x5145, 0x0000 },
    { 0x40f7, 0x01fd, 0x5146, 0x0000 },
    { 0x00fd, 0x0003, 0x5165, 0x0000 },
    { 0x4001, 0x01fa, 0x51a5, 0x0000 },
    { 0x80fa, 0x01f6, 0x5184, 0xffff },
};
#pragma section obj end
