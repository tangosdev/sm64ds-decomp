/* Sprite template list, 4 OamAttrs. */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data.gap15b" ".data.gap15b" ".data.gap15b" far_data
#pragma section obj begin
struct OamAttr data_ov006_02134ed0[4] = {
    { 0x00f4, 0x41f1, 0x2049, 0x0000 },
    { 0x80f4, 0x0001, 0x204b, 0x0000 },
    { 0x4004, 0x01f1, 0x2046, 0x0000 },
    { 0x0004, 0x0001, 0x2048, 0xffff },
};
#pragma section obj end
