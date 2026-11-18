/*
 * Minigame-overlay data at 0x0213ca9c..0x0213cab8.
 *
 * Sprite templates are OamAttr lists (terminated by 0xffff in
 * attr3); the tables that index them hold OamAttr pointers. A
 * word the module relocates is written as its target symbol so
 * the link re-creates the relocation; the rest are the
 * cartridge words.
 */
#include "types.h"

extern u8 _ZTI15dScMgHanachan_c[];
extern struct OamAttr * sMgTextSpritesEnglish[];
extern struct OamAttr * sMgTextSpritesFrench[];
extern struct OamAttr * sMgTextSpritesGerman[];
extern struct OamAttr * sMgTextSpritesItalian[];
extern struct OamAttr * sMgTextSpritesSpanish[];

#pragma define_section obj ".data" ".data" ".data" far_data
#pragma section obj begin
void * sHanaTextSpritesByLanguage[7] = {
    (void *)&sMgTextSpritesEnglish,
    (void *)&sMgTextSpritesSpanish,
    (void *)&sMgTextSpritesFrench,
    (void *)&sMgTextSpritesGerman,
    (void *)&sMgTextSpritesItalian,
    0,
    (void *)&_ZTI15dScMgHanachan_c,
};
#pragma section obj end
