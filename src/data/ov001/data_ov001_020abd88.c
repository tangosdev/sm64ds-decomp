/* Sprite template list, 2 OamAttrs. */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data" ".data" ".data" far_data
#pragma section obj begin
struct OamAttr _ZN3OAM14BOUNCING_ARROWE[2] = {
    { 0x00f4, 0x41f8, 0x1080, 0x0000 },
    { 0x4004, 0x01f8, 0x10a2, 0xffff },
};
#pragma section obj end
