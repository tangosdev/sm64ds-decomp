/*
 * Minigame-overlay data at 0x0213c4dc..0x0213c510.
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
extern struct OamAttr * sMgTextSpritesItalian[];
extern struct OamAttr * sMgTextSpritesSpanish[];

#pragma define_section obj ".data.w13b814" ".data.w13b814" ".data.w13b814" far_data
#pragma section obj begin
struct OamAttr ** sCurling2CounterSprites[5] = {
    (struct OamAttr **)&sMgTextSpritesEnglish,
    (struct OamAttr **)&sMgTextSpritesSpanish,
    (struct OamAttr **)&sMgTextSpritesFrench,
    (struct OamAttr **)&sMgTextSpritesGerman,
    (struct OamAttr **)&sMgTextSpritesItalian,
};
#pragma section obj end
