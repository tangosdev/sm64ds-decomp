/* Sprite template list, 4 OamAttrs. */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data" ".data" ".data" far_data
#pragma section obj begin
struct OamAttr _ZN3OAM19PLAYER_SELECT_MARIOE[4] = {
    { 0x20e8, 0xc1d8, 0x0240, 0x0000 },
    { 0x20e8, 0xc1e8, 0x0244, 0x0000 },
    { 0x20d8, 0xc1d8, 0x0200, 0x0000 },
    { 0x20d8, 0xc1e8, 0x0204, 0xffff },
};
#pragma section obj end
