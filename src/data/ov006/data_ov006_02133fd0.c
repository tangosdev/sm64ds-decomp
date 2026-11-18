/* Sprite template list, 2 OamAttrs. */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data.gap6" ".data.gap6" ".data.gap6" far_data
#pragma section obj begin
struct OamAttr data_ov006_02133fd0[2] = {
    { 0x40f8, 0x81e0, 0x00c0, 0x0000 },
    { 0x40f8, 0x8000, 0x00c4, 0xffff },
};
#pragma section obj end
