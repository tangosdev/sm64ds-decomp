/* Sprite template list, 4 OamAttrs. */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data" ".data" ".data" far_data
#pragma section obj begin
struct OamAttr data_ov001_020abc94[4] = {
    { 0x00c0, 0x41f8, 0xa01a, 0x0000 },
    { 0x00b8, 0x81e8, 0x9008, 0x0000 },
    { 0x80b8, 0x8008, 0x900c, 0x0000 },
    { 0x00d8, 0x41f8, 0x908a, 0xffff },
};
#pragma section obj end
