/*
 * Minigame-overlay data at 0x02136e2c..0x02136e38.
 *
 * Sprite templates are OamAttr lists (terminated by 0xffff in
 * attr3); the tables that index them hold OamAttr pointers. A
 * word the module relocates is written as its target symbol so
 * the link re-creates the relocation; the rest are the
 * cartridge words.
 */
#include "types.h"

extern struct OamAttr data_ov006_02136e30;

#pragma define_section obj ".data.gap23" ".data.gap23" ".data.gap23" far_data
#pragma section obj begin
struct OamAttr * sPanelBannerSprites[1] = { (struct OamAttr *)&data_ov006_02136e30 };
#pragma section obj end
