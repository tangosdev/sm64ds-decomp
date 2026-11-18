/* Pointers, 5 entries. */
#include "types.h"

extern struct OamAttr * sMgTextSpritesEnglish[];
extern struct OamAttr * sMgTextSpritesFrench[];
extern struct OamAttr * sMgTextSpritesGerman[];
extern struct OamAttr * sMgTextSpritesItalian[];
extern struct OamAttr * sMgTextSpritesSpanish[];

#pragma define_section obj ".data" ".data" ".data" far_data
#pragma section obj begin
void ** sSlotLangSprites[5] = {
    (void **)&sMgTextSpritesEnglish,
    (void **)&sMgTextSpritesSpanish,
    (void **)&sMgTextSpritesFrench,
    (void **)&sMgTextSpritesGerman,
    (void **)&sMgTextSpritesItalian,
};
#pragma section obj end
