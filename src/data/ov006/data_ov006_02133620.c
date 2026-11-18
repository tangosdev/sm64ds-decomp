/* Sprite template list, 2 OamAttrs. */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data.gap0" ".data.gap0" ".data.gap0" far_data
#pragma section obj begin
struct OamAttr data_ov006_02133620[2] = {
    { 0x00e8, 0x81f0, 0x0000, 0x0000 },
    { 0x4008, 0x81f0, 0x0080, 0xffff },
};
#pragma section obj end
