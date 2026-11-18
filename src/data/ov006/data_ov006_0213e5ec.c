/* Pointers, 5 entries. */
#include "types.h"

extern struct OamAttr *sMgTextSpritesEnglish[];
extern struct OamAttr *sMgTextSpritesFrench[];
extern struct OamAttr *sMgTextSpritesGerman[];
extern struct OamAttr *sMgTextSpritesItalian[];
extern struct OamAttr *sMgTextSpritesSpanish[];

void **sSlotLangSprites[5] = {
    (void **)&sMgTextSpritesEnglish, (void **)&sMgTextSpritesSpanish,
    (void **)&sMgTextSpritesFrench,  (void **)&sMgTextSpritesGerman,
    (void **)&sMgTextSpritesItalian,
};
