/* Sprite template list, 4 OamAttrs. */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data.gap32" ".data.gap32" ".data.gap32" far_data
#pragma section obj begin
struct OamAttr data_ov006_02138a68[4] = {
    { 0x00e8, 0x81e8, 0x3012, 0x0000 },
    { 0x80e8, 0x8008, 0x3016, 0x0000 },
    { 0x4008, 0x81e8, 0x3092, 0x0000 },
    { 0x0008, 0x4008, 0x3096, 0xffff },
};
#pragma section obj end
