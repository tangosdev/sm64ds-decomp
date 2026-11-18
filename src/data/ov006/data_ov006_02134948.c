/* Sprite template list, 4 OamAttrs. */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data.gap13" ".data.gap13" ".data.gap13" far_data
#pragma section obj begin
struct OamAttr data_ov006_02134948[4] = {
    { 0x00f8, 0x41e5, 0xc300, 0x0000 },
    { 0x00f8, 0x41f3, 0xc302, 0x0000 },
    { 0x00f8, 0x4001, 0xc304, 0x0000 },
    { 0x00f8, 0x400d, 0xc306, 0xffff },
};
#pragma section obj end
