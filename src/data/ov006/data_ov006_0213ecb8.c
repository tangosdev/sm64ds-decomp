/*
 * Minigame-overlay data at 0x0213ecb8..0x0213ece8.
 *
 * Sprite templates are OamAttr lists (terminated by 0xffff in
 * attr3); the tables that index them hold OamAttr pointers. A
 * word the module relocates is written as its target symbol so
 * the link re-creates the relocation; the rest are the
 * cartridge words.
 */
#include "types.h"

extern struct OamAttr *sMgTextSpritesEnglish[];
extern struct OamAttr *sMgTextSpritesFrench[];
extern struct OamAttr *sMgTextSpritesGerman[];
extern struct OamAttr *sMgTextSpritesItalian[];
extern struct OamAttr *sMgTextSpritesSpanish[];

struct OamAttr **sSmartballTextSpritesByLanguage[5] = {
    (struct OamAttr **)&sMgTextSpritesEnglish, (struct OamAttr **)&sMgTextSpritesSpanish,
    (struct OamAttr **)&sMgTextSpritesFrench,  (struct OamAttr **)&sMgTextSpritesGerman,
    (struct OamAttr **)&sMgTextSpritesItalian,
};
