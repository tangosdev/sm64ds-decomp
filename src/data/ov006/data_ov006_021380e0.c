/* Sprite template list, 3 OamAttrs. */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data.gap31" ".data.gap31" ".data.gap31" far_data
#pragma section obj begin
struct OamAttr data_ov006_021380e0[3] = {
    { 0x80d8, 0x41fd, 0x6107, 0x0000 },
    { 0x8008, 0x61fd, 0x6107, 0x0000 },
    { 0x80f8, 0x01fd, 0x6127, 0xffff },
};
#pragma section obj end
