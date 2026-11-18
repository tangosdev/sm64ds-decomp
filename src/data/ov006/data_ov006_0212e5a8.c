/*
 * Minigame-overlay read-only table at 0x0212e5a8..0x0212e80c.
 *
 * Values are the cartridge words; a word the module relocates
 * is written as its target symbol so the link re-creates it.
 */
#include "types.h"

extern u32 data_ov006_0213c75c[];

#pragma define_section obj ".rodata.gap2" ".rodata.gap2" ".rodata.gap2" far_data
#pragma section obj begin
u32 data_ov006_0212e5a8[7] = {
    0x0000ffff,
    (u32)&data_ov006_0213c75c,
    0x00000001,
    0x00000001,
    0x00000001,
    0x00000078,
    0x00000001,
};
#pragma section obj end
