/* Sprite template list, 2 OamAttrs. */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data.gap22" ".data.gap22" ".data.gap22" far_data
#pragma section obj begin
struct OamAttr data_ov006_02136cfc[2] = {
    { 0x80f0, 0x81f8, 0x1004, 0x0000 },
    { 0x80f0, 0x4005, 0x1006, 0xffff },
};
#pragma section obj end
