/* Sprite template list, 2 OamAttrs. */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data" ".data" ".data" far_data
#pragma section obj begin
struct OamAttr _ZN3OAM20MM_WARIO_NO_CAP_ICONE[2] = {
    { 0x4000, 0x81f0, 0xc1c8, 0x0000 },
    { 0x40f0, 0x81f0, 0xc198, 0xffff },
};
#pragma section obj end
