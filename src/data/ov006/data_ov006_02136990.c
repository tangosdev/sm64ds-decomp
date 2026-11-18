/* Sprite template list, 2 OamAttrs. */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data.gap21" ".data.gap21" ".data.gap21" far_data
#pragma section obj begin
struct OamAttr data_ov006_02136990[2] = {
    { 0x00f8, 0x41f8, 0x10c0, 0x0000 },
    { 0x40f8, 0x81f0, 0x0122, 0xffff },
};
#pragma section obj end
