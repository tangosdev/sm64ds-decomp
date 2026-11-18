/* Sprite template list, 3 OamAttrs. */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data.gap24" ".data.gap24" ".data.gap24" far_data
#pragma section obj begin
struct OamAttr data_ov006_021371dc[3] = {
    { 0x0003, 0x41f9, 0xf214, 0x0000 },
    { 0x40f0, 0x81f0, 0xf210, 0x0000 },
    { 0x4000, 0x81f0, 0xf207, 0xffff },
};
#pragma section obj end
