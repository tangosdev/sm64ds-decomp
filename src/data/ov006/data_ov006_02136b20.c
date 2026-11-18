/* Sprite template list, 4 OamAttrs. */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data.gap21" ".data.gap21" ".data.gap21" far_data
#pragma section obj begin
struct OamAttr data_ov006_02136b20[4] = {
    { 0x40f1, 0x81f0, 0x0000, 0x0000 },
    { 0x4001, 0x41f0, 0x0040, 0x0000 },
    { 0x0009, 0x01f8, 0x006d, 0x0000 },
    { 0x0009, 0x0000, 0x00ee, 0xffff },
};
#pragma section obj end
