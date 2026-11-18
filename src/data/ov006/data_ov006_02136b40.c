/* Sprite template list, 4 OamAttrs. */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data.gap21" ".data.gap21" ".data.gap21" far_data
#pragma section obj begin
struct OamAttr data_ov006_02136b40[4] = {
    { 0x40f0, 0x81f0, 0x000c, 0x0000 },
    { 0x4000, 0x41f0, 0x004c, 0x0000 },
    { 0x4008, 0x41f0, 0x006c, 0x0000 },
    { 0x40f0, 0x11e8, 0x2156, 0xffff },
};
#pragma section obj end
