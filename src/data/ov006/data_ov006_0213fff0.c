/*
 * Minigame-overlay data at 0x0213fff0..0x0214000c.
 *
 * Sprite templates are OamAttr lists (terminated by 0xffff in
 * attr3); the tables that index them hold OamAttr pointers. A
 * word the module relocates is written as its target symbol so
 * the link re-creates the relocation; the rest are the
 * cartridge words.
 */
#include "types.h"

extern struct OamAttr * sMgTextSpritesEnglish[];
extern struct OamAttr * sMgTextSpritesFrench[];
extern struct OamAttr * sMgTextSpritesGerman[];
extern struct OamAttr * sMgTextSpritesSpanish[];

#pragma define_section obj ".data" ".data" ".data" far_data
#pragma section obj begin
void ** sSnowballTextSpritesByLanguage[4] = {
    (void **)&sMgTextSpritesEnglish,
    (void **)&sMgTextSpritesSpanish,
    (void **)&sMgTextSpritesFrench,
    (void **)&sMgTextSpritesGerman,
};
#pragma section obj end
