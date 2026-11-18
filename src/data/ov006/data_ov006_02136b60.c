/* Sprite template list, 4 OamAttrs. */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data.gap21" ".data.gap21" ".data.gap21" far_data
#pragma section obj begin
struct OamAttr data_ov006_02136b60[4] = {
    { 0x40ef, 0x81f1, 0x0000, 0x0000 },
    { 0x40ff, 0x41f1, 0x0040, 0x0000 },
    { 0x4006, 0x01f8, 0x00ed, 0x0000 },
    { 0x0007, 0x0008, 0x00ef, 0xffff },
};
#pragma section obj end
