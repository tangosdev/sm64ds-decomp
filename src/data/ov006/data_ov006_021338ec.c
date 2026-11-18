/* Sprite template list, 2 OamAttrs. */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data.gap1" ".data.gap1" ".data.gap1" far_data
#pragma section obj begin
struct OamAttr data_ov006_021338ec[2] = {
    { 0x40f8, 0x81f0, 0x1118, 0x0000 },
    { 0x80f8, 0x0010, 0x111c, 0xffff },
};
#pragma section obj end
