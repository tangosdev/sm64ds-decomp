/* Sprite template list, 3 OamAttrs. */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data.gap31" ".data.gap31" ".data.gap31" far_data
#pragma section obj begin
struct OamAttr data_ov006_02138168[3] = {
    { 0x00d8, 0x9008, 0x6084, 0x0000 },
    { 0x0008, 0xa1d8, 0x6084, 0x0000 },
    { 0x00f0, 0x91f0, 0x6084, 0xffff },
};
#pragma section obj end
