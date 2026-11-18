/* Sprite template list, 4 OamAttrs. */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data" ".data" ".data" far_data
#pragma section obj begin
struct OamAttr data_ov006_0213e788[4] = {
    { 0x40e0, 0x81e0, 0x2004, 0x0000 },
    { 0x40f0, 0x41e0, 0x2044, 0x0000 },
    { 0x40e0, 0x9000, 0x2004, 0x0000 },
    { 0x40f0, 0x5000, 0x2044, 0xffff },
};
#pragma section obj end
