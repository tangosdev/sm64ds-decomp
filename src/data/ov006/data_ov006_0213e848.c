/* Sprite template list, 4 OamAttrs. */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data" ".data" ".data" far_data
#pragma section obj begin
struct OamAttr data_ov006_0213e848[4] = {
    { 0x00e8, 0x81e8, 0x000c, 0x0000 },
    { 0x80e8, 0x8008, 0x0010, 0x0000 },
    { 0x4008, 0x81e8, 0x008c, 0x0000 },
    { 0x0008, 0x4008, 0x0090, 0xffff },
};
#pragma section obj end
