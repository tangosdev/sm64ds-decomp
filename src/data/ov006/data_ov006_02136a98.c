/* Sprite template list, 2 OamAttrs. */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data.gap21" ".data.gap21" ".data.gap21" far_data
#pragma section obj begin
struct OamAttr data_ov006_02136a98[2] = {
    { 0x00f0, 0x81f0, 0x0004, 0x0000 },
    { 0x40f6, 0x0005, 0x2156, 0xffff },
};
#pragma section obj end
