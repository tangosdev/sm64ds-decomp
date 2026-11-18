/* Sprite template list, 4 OamAttrs. */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data.gap14" ".data.gap14" ".data.gap14" far_data
#pragma section obj begin
struct OamAttr data_ov006_02134cfc[4] = {
    { 0x00f4, 0x41f1, 0x1015, 0x0000 },
    { 0x80f4, 0x0001, 0x1017, 0x0000 },
    { 0x4004, 0x01f1, 0x1012, 0x0000 },
    { 0x0004, 0x0001, 0x1014, 0xffff },
};
#pragma section obj end
