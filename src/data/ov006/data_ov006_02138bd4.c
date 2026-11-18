/* Sprite template list, 4 OamAttrs. */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data.gap32" ".data.gap32" ".data.gap32" far_data
#pragma section obj begin
struct OamAttr data_ov006_02138bd4[4] = {
    { 0x40f8, 0x81d4, 0x70cd, 0x0000 },
    { 0x40f8, 0x81f4, 0x70d1, 0x0000 },
    { 0x00f8, 0x4014, 0x70d5, 0x0000 },
    { 0x80f8, 0x0024, 0x70d7, 0xffff },
};
#pragma section obj end
