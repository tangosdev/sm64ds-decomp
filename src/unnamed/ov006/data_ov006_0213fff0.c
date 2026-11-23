#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    void *f000;
    void *f004;
    void *f008;
    void *f00c;
    void *f010;
    u32 f014;
    void *f018;
} sSnowballTextSpritesByLanguage_Layout;
typedef char
    sSnowballTextSpritesByLanguage_size[sizeof(sSnowballTextSpritesByLanguage_Layout) == 0x1c ? 1
                                                                                              : -1];

extern sSnowballTextSpritesByLanguage_Layout sSnowballTextSpritesByLanguage;
extern u8 sMgTextSpritesEnglish[];
extern u8 sMgTextSpritesSpanish[];
extern u8 sMgTextSpritesFrench[];
extern u8 sMgTextSpritesGerman[];
extern u8 sMgTextSpritesItalian[];
extern u8 _ZTI15dScMgSnowball_c[];

sSnowballTextSpritesByLanguage_Layout sSnowballTextSpritesByLanguage = {
    &sMgTextSpritesEnglish[0], &sMgTextSpritesSpanish[0], &sMgTextSpritesFrench[0],
    &sMgTextSpritesGerman[0],  &sMgTextSpritesItalian[0], 0x00000000,
    &_ZTI15dScMgSnowball_c[0],
};
