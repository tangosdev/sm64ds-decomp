/* Sprite template list, 3 OamAttrs. */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data.gap24" ".data.gap24" ".data.gap24" far_data
#pragma section obj begin
struct OamAttr data_ov006_02137194[3] = {
    { 0x40f8, 0x81dc, 0xe2d7, 0x0000 },
    { 0x40f8, 0x81fc, 0xe2db, 0x0000 },
    { 0x80f8, 0x001c, 0xe2df, 0xffff },
};
#pragma section obj end
