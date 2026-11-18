/* Sprite template list, 6 OamAttrs. */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data.gap32" ".data.gap32" ".data.gap32" far_data
#pragma section obj begin
struct OamAttr data_ov006_02138af8[6] = {
    { 0x40fd, 0x01e8, 0x61fc, 0x0000 },
    { 0x40fd, 0x01f8, 0x61fd, 0x0000 },
    { 0x40fd, 0x0008, 0x61fe, 0x0000 },
    { 0x00f8, 0x41ed, 0x71da, 0x0000 },
    { 0x00f8, 0x41f8, 0x71da, 0x0000 },
    { 0x00f8, 0x4003, 0x71da, 0xffff },
};
#pragma section obj end
