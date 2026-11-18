/* Sprite template list, 5 OamAttrs. */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data" ".data" ".data" far_data
#pragma section obj begin
struct OamAttr data_ov006_0213e0e4[5] = {
    { 0x00f2, 0x41f1, 0x20ba, 0x0000 },
    { 0x00f3, 0x41f5, 0x2019, 0x0000 },
    { 0x80f3, 0x0005, 0x201b, 0x0000 },
    { 0x4003, 0x01f5, 0x2059, 0x0000 },
    { 0x0003, 0x0005, 0x205b, 0xffff },
};
#pragma section obj end
