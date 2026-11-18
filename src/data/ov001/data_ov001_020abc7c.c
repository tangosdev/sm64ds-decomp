/* Sprite template list, 3 OamAttrs. */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data" ".data" ".data" far_data
#pragma section obj begin
struct OamAttr data_ov001_020abc7c[3] = {
    { 0x00e8, 0x81e8, 0x9014, 0x0000 },
    { 0x80e8, 0x8008, 0x9018, 0x0000 },
    { 0x0008, 0x41f8, 0x9096, 0xffff },
};
#pragma section obj end
