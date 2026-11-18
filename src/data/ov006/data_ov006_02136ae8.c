/* Sprite template list, 3 OamAttrs. */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data.gap21" ".data.gap21" ".data.gap21" far_data
#pragma section obj begin
struct OamAttr data_ov006_02136ae8[3] = {
    { 0x4007, 0x41f0, 0x006c, 0x0000 },
    { 0x40ef, 0x81ef, 0x0000, 0x0000 },
    { 0x40ff, 0x41ef, 0x0040, 0xffff },
};
#pragma section obj end
