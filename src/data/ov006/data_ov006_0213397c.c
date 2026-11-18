/* Sprite template list, 4 OamAttrs. */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data.gap1" ".data.gap1" ".data.gap1" far_data
#pragma section obj begin
struct OamAttr data_ov006_0213397c[4] = {
    { 0x00e8, 0x81e8, 0x0158, 0x0000 },
    { 0x80e8, 0x8008, 0x015c, 0x0000 },
    { 0x4008, 0x81e8, 0x01d8, 0x0000 },
    { 0x0008, 0x4008, 0x01dc, 0xffff },
};
#pragma section obj end
