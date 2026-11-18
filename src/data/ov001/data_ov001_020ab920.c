/* Sprite template list, 3 OamAttrs. */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data" ".data" ".data" far_data
#pragma section obj begin
struct OamAttr _ZN3OAM19MM_YOSHI_W_CAP_ICONE[3] = {
    { 0x00ea, 0x41f4, 0xc106, 0x0000 },
    { 0x80ea, 0x0004, 0xc108, 0x0000 },
    { 0x00f0, 0x81f0, 0xd18c, 0xffff },
};
#pragma section obj end
