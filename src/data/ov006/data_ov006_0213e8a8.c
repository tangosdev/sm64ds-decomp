/* Sprite template list, 4 OamAttrs. */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data" ".data" ".data" far_data
#pragma section obj begin
struct OamAttr data_ov006_0213e8a8[4] = {
    { 0x00e8, 0x81e8, 0x0018, 0x0000 },
    { 0x80e8, 0x8008, 0x001c, 0x0000 },
    { 0x4008, 0x81e8, 0x0098, 0x0000 },
    { 0x0008, 0x4008, 0x009c, 0xffff },
};
#pragma section obj end
