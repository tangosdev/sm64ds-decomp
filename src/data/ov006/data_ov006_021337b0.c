/* Sprite template list, 3 OamAttrs. */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data.gap0" ".data.gap0" ".data.gap0" far_data
#pragma section obj begin
struct OamAttr data_ov006_021337b0[3] = {
    { 0x00f0, 0x81f0, 0x2006, 0x0000 },
    { 0x00e8, 0x81f0, 0x0112, 0x0000 },
    { 0x4008, 0x81f0, 0x0192, 0xffff },
};
#pragma section obj end
