/* Sprite template list, 4 OamAttrs. */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data" ".data" ".data" far_data
#pragma section obj begin
struct OamAttr data_ov006_0213e7c8[4] = {
    { 0x4010, 0x81e0, 0x20c4, 0x0000 },
    { 0x4020, 0x41e0, 0x2104, 0x0000 },
    { 0x4010, 0x9000, 0x20c4, 0x0000 },
    { 0x4020, 0x5000, 0x2104, 0xffff },
};
#pragma section obj end
