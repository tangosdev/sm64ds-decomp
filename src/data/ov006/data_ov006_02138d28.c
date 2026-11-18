/*
 * Minigame-overlay data at 0x02138d28..0x02139db4.
 *
 * Sprite templates are OamAttr lists (terminated by 0xffff in
 * attr3); the tables that index them hold OamAttr pointers. A
 * word the module relocates is written as its target symbol so
 * the link re-creates the relocation; the rest are the
 * cartridge words.
 */
#include "types.h"

extern struct OamAttr data_ov006_02138d34[];
extern struct OamAttr data_ov006_02138d44[];
extern struct OamAttr data_ov006_02138d54[];

#pragma define_section obj ".data.gap33" ".data.gap33" ".data.gap33" far_data
#pragma section obj begin
struct OamAttr * sSoundWinSpriteASprites[3] = {
    (struct OamAttr *)&data_ov006_02138d44,
    (struct OamAttr *)&data_ov006_02138d34,
    (struct OamAttr *)&data_ov006_02138d54,
};
#pragma section obj end
