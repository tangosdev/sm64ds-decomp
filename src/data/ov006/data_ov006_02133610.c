/* Sprite template list, 2 OamAttrs. */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data.gap0" ".data.gap0" ".data.gap0" far_data
#pragma section obj begin
struct OamAttr data_ov006_02133610[2] = {
    { 0x80e8, 0x81f8, 0x0004, 0x0000 },
    { 0x0008, 0x41f8, 0x0084, 0xffff },
};
#pragma section obj end
