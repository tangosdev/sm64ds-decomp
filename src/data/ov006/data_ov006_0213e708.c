/* Sprite template list, 4 OamAttrs. */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data" ".data" ".data" far_data
#pragma section obj begin
struct OamAttr data_ov006_0213e708[4] = {
    { 0x4010, 0x81e0, 0x00c0, 0x0000 },
    { 0x4020, 0x41e0, 0x0100, 0x0000 },
    { 0x4010, 0x9000, 0x00c0, 0x0000 },
    { 0x4020, 0x5000, 0x0100, 0xffff },
};
#pragma section obj end
