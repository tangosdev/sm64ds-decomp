/* Sprite template list, 5 OamAttrs. */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data.gap31" ".data.gap31" ".data.gap31" far_data
#pragma section obj begin
struct OamAttr data_ov006_02137f90[5] = {
    { 0x00f8, 0x01f8, 0x61dc, 0x0000 },
    { 0x0000, 0x0000, 0x61dc, 0x0000 },
    { 0x00f8, 0x0000, 0x61dc, 0x0000 },
    { 0x0000, 0x01f8, 0x61dc, 0x0000 },
    { 0x00f0, 0x81f0, 0x2000, 0xffff },
};
#pragma section obj end
