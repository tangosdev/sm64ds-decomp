/* Sprite template list, 5 OamAttrs. */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data.gap13" ".data.gap13" ".data.gap13" far_data
#pragma section obj begin
struct OamAttr data_ov006_02134968[5] = {
    { 0x00f8, 0x41df, 0xc300, 0x0000 },
    { 0x00f8, 0x41ef, 0xc302, 0x0000 },
    { 0x00f8, 0x41fd, 0xc304, 0x0000 },
    { 0x00f8, 0x4007, 0xc306, 0x0000 },
    { 0x00f8, 0x4014, 0xc308, 0xffff },
};
#pragma section obj end
