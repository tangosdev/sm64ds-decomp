/* Sprite template list, 3 OamAttrs. */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data.gap31" ".data.gap31" ".data.gap31" far_data
#pragma section obj begin
struct OamAttr data_ov006_021380c8[3] = {
    { 0x40fc, 0x41d8, 0x61fc, 0x0000 },
    { 0x40fc, 0x5008, 0x61fc, 0x0000 },
    { 0x40fc, 0x01f8, 0x61fd, 0xffff },
};
#pragma section obj end
