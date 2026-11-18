/* Sprite template list, 3 OamAttrs. */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data" ".data" ".data" far_data
#pragma section obj begin
struct OamAttr data_ov006_0213ddf4[3] = {
    { 0x00f5, 0x0005, 0x2079, 0x0000 },
    { 0x00f8, 0x41f8, 0x207a, 0x0000 },
    { 0x00f5, 0x01f5, 0x2099, 0xffff },
};
#pragma section obj end
