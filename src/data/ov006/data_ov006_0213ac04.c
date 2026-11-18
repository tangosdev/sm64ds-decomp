/* Sprite template list, 4 OamAttrs. */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data.gap43" ".data.gap43" ".data.gap43" far_data
#pragma section obj begin
struct OamAttr data_ov006_0213ac04[4] = {
    { 0x80e8, 0x8000, 0x0024, 0x0000 },
    { 0x0008, 0x4000, 0x00a4, 0x0000 },
    { 0x00e0, 0x0000, 0x0004, 0x0000 },
    { 0x80f8, 0x0010, 0x0066, 0xffff },
};
#pragma section obj end
