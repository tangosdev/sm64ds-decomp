/* Sprite template list, 4 OamAttrs. */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data.gap32" ".data.gap32" ".data.gap32" far_data
#pragma section obj begin
struct OamAttr data_ov006_02138c1c[4] = {
    { 0x00ec, 0x81e8, 0x0000, 0x0000 },
    { 0x80ec, 0x9008, 0x0000, 0x0000 },
    { 0x400c, 0x41e8, 0x0004, 0x0000 },
    { 0x400c, 0x0008, 0x0024, 0xffff },
};
#pragma section obj end
