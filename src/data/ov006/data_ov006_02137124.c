/* Sprite template list, 2 OamAttrs. */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data.gap24" ".data.gap24" ".data.gap24" far_data
#pragma section obj begin
struct OamAttr data_ov006_02137124[2] = {
    { 0x0006, 0x41fb, 0xf214, 0x0000 },
    { 0x00f0, 0x81f0, 0xf210, 0xffff },
};
#pragma section obj end
