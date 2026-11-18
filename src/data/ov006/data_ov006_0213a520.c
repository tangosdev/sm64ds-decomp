/* Sprite template list, 6 OamAttrs. */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data.gap38" ".data.gap38" ".data.gap38" far_data
#pragma section obj begin
struct OamAttr data_ov006_0213a520[6] = {
    { 0x4006, 0xc1e0, 0x30aa, 0x0000 },
    { 0x4026, 0x41e0, 0x312a, 0x0000 },
    { 0x4026, 0x4000, 0x312e, 0x0000 },
    { 0x4028, 0x01e8, 0x40dc, 0x0000 },
    { 0x4028, 0x0008, 0x40de, 0x0000 },
    { 0x4028, 0x01f8, 0x40dd, 0xffff },
};
#pragma section obj end
