/* Sprite template list, 3 OamAttrs. */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data.gap0" ".data.gap0" ".data.gap0" far_data
#pragma section obj begin
struct OamAttr data_ov006_021336a8[3] = {
    { 0x80f0, 0x81f8, 0x2090, 0x0000 },
    { 0x80e8, 0x81f8, 0x0116, 0x0000 },
    { 0x0008, 0x41f8, 0x0196, 0xffff },
};
#pragma section obj end
