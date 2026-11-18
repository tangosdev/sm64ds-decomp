/* Sprite template list, 4 OamAttrs. */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data.gap38" ".data.gap38" ".data.gap38" far_data
#pragma section obj begin
struct OamAttr data_ov006_0213a33c[4] = {
    { 0x40f8, 0x81ec, 0x009b, 0x0000 },
    { 0x80f8, 0x000c, 0x009f, 0x0000 },
    { 0x00f0, 0x81e0, 0x001b, 0x0000 },
    { 0x00f0, 0x8000, 0x001c, 0xffff },
};
#pragma section obj end
