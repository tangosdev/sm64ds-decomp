/* Sprite template list, 5 OamAttrs. */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data.gap10" ".data.gap10" ".data.gap10" far_data
#pragma section obj begin
struct OamAttr data_ov006_02134224[5] = {
    { 0x00f7, 0x01f5, 0x11ef, 0x0000 },
    { 0x00f1, 0x01fc, 0x11ef, 0x0000 },
    { 0x0000, 0x01fa, 0x11ef, 0x0000 },
    { 0x00fd, 0x0002, 0x11ef, 0x0000 },
    { 0x00f5, 0x0002, 0x11ef, 0xffff },
};
#pragma section obj end
