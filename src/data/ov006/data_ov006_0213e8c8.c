/* Sprite template list, 4 OamAttrs. */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data" ".data" ".data" far_data
#pragma section obj begin
struct OamAttr data_ov006_0213e8c8[4] = {
    { 0x40f8, 0x81e0, 0x406c, 0x0000 },
    { 0x4008, 0x41e0, 0x40ac, 0x0000 },
    { 0x40f8, 0x9000, 0x406c, 0x0000 },
    { 0x4008, 0x5000, 0x40ac, 0xffff },
};
#pragma section obj end
