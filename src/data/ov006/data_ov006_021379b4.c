/* Sprite template list, 5 OamAttrs. */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data.gap31" ".data.gap31" ".data.gap31" far_data
#pragma section obj begin
struct OamAttr data_ov006_021379b4[5] = {
    { 0x80f8, 0x0004, 0x410f, 0x0000 },
    { 0x00f8, 0x000c, 0x4110, 0x0000 },
    { 0x0000, 0x000c, 0x3130, 0x0000 },
    { 0x00f8, 0x01ec, 0x410c, 0x0000 },
    { 0x0000, 0x01ec, 0x312c, 0xffff },
};
#pragma section obj end
