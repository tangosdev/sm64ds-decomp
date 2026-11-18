/* Sprite template list, 6 OamAttrs. */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data.gap13" ".data.gap13" ".data.gap13" far_data
#pragma section obj begin
struct OamAttr data_ov006_021347d4[6] = {
    { 0x00f8, 0x41d9, 0xc340, 0x0000 },
    { 0x00f8, 0x41e9, 0xc342, 0x0000 },
    { 0x00f8, 0x41f6, 0xc344, 0x0000 },
    { 0x00f8, 0x41ff, 0xc346, 0x0000 },
    { 0x00f8, 0x400d, 0xc348, 0x0000 },
    { 0x00f8, 0x401a, 0xc34a, 0xffff },
};
#pragma section obj end
