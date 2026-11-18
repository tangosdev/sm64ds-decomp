/* Sprite template list, 3 OamAttrs. */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data.gap38" ".data.gap38" ".data.gap38" far_data
#pragma section obj begin
struct OamAttr data_ov006_0213a3d0[3] = {
    { 0x00e0, 0x81ec, 0x300e, 0x0000 },
    { 0x80e0, 0x400c, 0x3012, 0x0000 },
    { 0x4000, 0x81f0, 0x205a, 0xffff },
};
#pragma section obj end
