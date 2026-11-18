/* Sprite template list, 4 OamAttrs. */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data" ".data" ".data" far_data
#pragma section obj begin
struct OamAttr sSlotsSlotBlinkSprites[4] = {
    { 0x40f4, 0x81e8, 0x1ccc, 0x0000 },
    { 0x4004, 0x41e8, 0x1cec, 0x0000 },
    { 0x00f4, 0x4008, 0x1cd0, 0x0000 },
    { 0x4004, 0x0008, 0x1cf0, 0xffff },
};
#pragma section obj end
