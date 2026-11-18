/* Sprite template list, 4 OamAttrs. */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data.gap38" ".data.gap38" ".data.gap38" far_data
#pragma section obj begin
struct OamAttr data_ov006_0213a578[4] = {
    { 0x00e8, 0x81e8, 0x300b, 0x0000 },
    { 0x80e8, 0x8008, 0x300f, 0x0000 },
    { 0x4008, 0x41e8, 0x308b, 0x0000 },
    { 0x4008, 0x0008, 0x308f, 0xffff },
};
#pragma section obj end
