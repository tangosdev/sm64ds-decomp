/* Sprite template list, 3 OamAttrs. */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data.gap21" ".data.gap21" ".data.gap21" far_data
#pragma section obj begin
struct OamAttr data_ov006_02136ab8[3] = {
    { 0x40f0, 0x91ef, 0x0080, 0x0000 },
    { 0x4000, 0x51ef, 0x00c0, 0x0000 },
    { 0x4008, 0x11fc, 0x00e4, 0xffff },
};
#pragma section obj end
