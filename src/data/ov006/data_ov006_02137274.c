/* Sprite template list, 4 OamAttrs. */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data.gap24" ".data.gap24" ".data.gap24" far_data
#pragma section obj begin
struct OamAttr data_ov006_02137274[4] = {
    { 0x00f8, 0x41e0, 0xa254, 0x0000 },
    { 0x40f8, 0x81f0, 0x8256, 0x0000 },
    { 0x40f8, 0x8010, 0x825a, 0x0000 },
    { 0x00f8, 0x4030, 0x825e, 0xffff },
};
#pragma section obj end
