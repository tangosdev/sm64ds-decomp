/* claimed-storage */
/* ov006 .data.gap55 0x0213cb8c..0x0213cbc8. 4 storage addresses, one object. */
#include "types.h"

extern u32 _ZTVN3abi20__si_class_type_infoE[];
extern u32 _ZTS11dScMgJump_c[];
extern u32 _ZTI14dScMgD3DBase_c[];
extern u32 sMgTextSpritesEnglish[];
extern u32 sMgTextSpritesSpanish[];
extern u32 sMgTextSpritesFrench[];
extern u32 sMgTextSpritesGerman[];
extern u32 sMgTextSpritesItalian[];

#pragma define_section claimsec ".data.gap55" ".data.gap55" ".data.gap55" far_data
#pragma section claimsec begin
#pragma explicit_zero_data on
u32 sJumpRenderScale[] = {
    0x00002800, 0x00002800, 0x00001000,
    (u32)&_ZTVN3abi20__si_class_type_infoE,
    (u32)&_ZTS11dScMgJump_c,
    (u32)&_ZTI14dScMgD3DBase_c,
    0x53643131, 0x4a674d63, 0x5f706d75, 0x00000063,
    (u32)&sMgTextSpritesEnglish,
    (u32)&sMgTextSpritesSpanish,
    (u32)&sMgTextSpritesFrench,
    (u32)&sMgTextSpritesGerman,
    (u32)&sMgTextSpritesItalian,
};
#pragma explicit_zero_data off
#pragma section claimsec end
