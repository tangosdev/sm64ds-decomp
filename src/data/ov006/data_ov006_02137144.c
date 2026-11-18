/* Sprite template list, 2 OamAttrs. */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data.gap24" ".data.gap24" ".data.gap24" far_data
#pragma section obj begin
struct OamAttr data_ov006_02137144[2] = {
    { 0x40f8, 0x81e0, 0xe2d8, 0x0000 },
    { 0x40f8, 0x8000, 0xe2dc, 0xffff },
};
#pragma section obj end
