/* Sprite template list, 4 OamAttrs. */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data.gap22" ".data.gap22" ".data.gap22" far_data
#pragma section obj begin
struct OamAttr data_ov006_02136d78[4] = {
    { 0x00ff, 0x41f9, 0x0016, 0x0000 },
    { 0x00f0, 0x41f9, 0x0016, 0x0000 },
    { 0x00e1, 0x41f9, 0x0016, 0x0000 },
    { 0x80e0, 0xd1f0, 0x001c, 0xffff },
};
#pragma section obj end
