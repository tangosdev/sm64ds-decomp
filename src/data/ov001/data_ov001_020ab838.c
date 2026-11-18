/* Sprite template list, 3 OamAttrs. */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data" ".data" ".data" far_data
#pragma section obj begin
struct OamAttr _ZN3OAM20MM_MARIO_NO_CAP_ICONE[3] = {
    { 0x00f0, 0x41f4, 0xa192, 0x0000 },
    { 0x80f0, 0x0004, 0xa194, 0x0000 },
    { 0x4000, 0x81f0, 0xa1c0, 0xffff },
};
#pragma section obj end
