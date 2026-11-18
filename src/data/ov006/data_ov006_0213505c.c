/* Sprite template list, 4 OamAttrs. */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data.gap18" ".data.gap18" ".data.gap18" far_data
#pragma section obj begin
struct OamAttr data_ov006_0213505c[4] = {
    { 0x40f8, 0x81c0, 0x0080, 0x0000 },
    { 0x40f8, 0x81e0, 0x0084, 0x0000 },
    { 0x40f8, 0x8000, 0x0088, 0x0000 },
    { 0x40f8, 0x8020, 0x008c, 0xffff },
};
#pragma section obj end
