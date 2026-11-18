/* Sprite template list, 4 OamAttrs. */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data" ".data" ".data" far_data
#pragma section obj begin
struct OamAttr data_ov006_0213e908[4] = {
    { 0x40f0, 0x81e8, 0x10cc, 0x0000 },
    { 0x00f0, 0x4008, 0x10d0, 0x0000 },
    { 0x4000, 0x41e8, 0x110c, 0x0000 },
    { 0x4000, 0x0008, 0x1110, 0xffff },
};
#pragma section obj end
