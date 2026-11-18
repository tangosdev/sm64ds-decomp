/* Sprite template list, 4 OamAttrs. */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data" ".data" ".data" far_data
#pragma section obj begin
struct OamAttr data_ov001_020ab988[4] = {
    { 0x00f4, 0x41f0, 0xa069, 0x0000 },
    { 0x80f4, 0x0000, 0xa06b, 0x0000 },
    { 0x4004, 0x01f0, 0xa0a9, 0x0000 },
    { 0x0004, 0x0000, 0xa0ab, 0xffff },
};
#pragma section obj end
