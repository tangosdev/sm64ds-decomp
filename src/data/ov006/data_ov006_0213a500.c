/* Sprite template list, 4 OamAttrs. */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data.gap38" ".data.gap38" ".data.gap38" far_data
#pragma section obj begin
struct OamAttr data_ov006_0213a500[4] = {
    { 0x000e, 0x81e8, 0x10c0, 0x0000 },
    { 0x800e, 0x8008, 0x10c4, 0x0000 },
    { 0x4028, 0x01f4, 0x40dc, 0x0000 },
    { 0x0028, 0x0004, 0x40df, 0xffff },
};
#pragma section obj end
