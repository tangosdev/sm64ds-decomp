/*
 * One object for sRouletteTextSpritesByLanguage, sRouletteSlowStops,
 * data_ov006_0213e34c (48 bytes): the linker starts every input section on a
 * 4-byte boundary, so these tables cannot be separate.
 */
#include "types.h"

extern struct OamAttr *sMgTextSpritesSpanish[];
extern struct OamAttr *sMgTextSpritesItalian[];
extern struct OamAttr *sMgTextSpritesGerman[];
extern struct OamAttr *sMgTextSpritesFrench[];
extern struct OamAttr *sMgTextSpritesEnglish[];
void **sRouletteTextSpritesByLanguage[12] = {
    (void **)&sMgTextSpritesEnglish,
    (void **)&sMgTextSpritesSpanish,
    (void **)&sMgTextSpritesFrench,
    (void **)&sMgTextSpritesGerman,
    (void **)&sMgTextSpritesItalian,
    (void **)0x1fff0aaa,
    (void **)0x4aaa3554,
    (void **)0x75545fff,
    (void **)0x9fff8aaa,
    (void **)0xcaaab554,
    (void **)0xf554dfff,
    (void **)0x00000000,
};
