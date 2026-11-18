/* Sprite template list, 2 OamAttrs. */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data.gap31" ".data.gap31" ".data.gap31" far_data
#pragma section obj begin
struct OamAttr data_ov006_021376ac[2] = {
    { 0x80ec, 0x41fc, 0x10a0, 0x0000 },
    { 0x000c, 0x01fc, 0x1120, 0xffff },
};
#pragma section obj end
