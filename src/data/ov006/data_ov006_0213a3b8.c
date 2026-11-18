/* Sprite template list, 3 OamAttrs. */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data.gap38" ".data.gap38" ".data.gap38" far_data
#pragma section obj begin
struct OamAttr data_ov006_0213a3b8[3] = {
    { 0x00e0, 0x81eb, 0x3004, 0x0000 },
    { 0x80e0, 0x400b, 0x3008, 0x0000 },
    { 0x4000, 0x81f0, 0x205a, 0xffff },
};
#pragma section obj end
