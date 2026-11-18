/* Sprite template list, 4 OamAttrs. */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data" ".data" ".data" far_data
#pragma section obj begin
struct OamAttr data_ov001_020ab9a8[4] = {
    { 0x00f4, 0x41f0, 0xb06c, 0x0000 },
    { 0x80f4, 0x0000, 0xb06e, 0x0000 },
    { 0x4004, 0x01f0, 0xb0ac, 0x0000 },
    { 0x0004, 0x0000, 0xb0ae, 0xffff },
};
#pragma section obj end
