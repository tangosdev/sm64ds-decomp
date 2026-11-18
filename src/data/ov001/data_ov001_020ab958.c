/* Sprite template list, 2 OamAttrs. */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data" ".data" ".data" far_data
#pragma section obj begin
struct OamAttr data_ov001_020ab958[2] = {
    { 0x00f8, 0x41f0, 0xd072, 0x0000 },
    { 0x80f8, 0x0000, 0xd074, 0xffff },
};
#pragma section obj end
