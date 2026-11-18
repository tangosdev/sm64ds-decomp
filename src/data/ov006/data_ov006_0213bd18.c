/*
 * Minigame-overlay data at 0x0213bd18..0x0213bdb4.
 *
 * Sprite templates are OamAttr lists (terminated by 0xffff in
 * attr3); the tables that index them hold OamAttr pointers. A
 * word the module relocates is written as its target symbol so
 * the link re-creates the relocation; the rest are the
 * cartridge words.
 */
#include "types.h"

#pragma define_section obj ".data.w13b814" ".data.w13b814" ".data.w13b814" far_data
#pragma section obj begin
int sCardFaceWeights[6] = {
    0x00000005,
    0x00000005,
    0x00000005,
    0x00000005,
    0x00000005,
    0x00000005,
};
#pragma section obj end
