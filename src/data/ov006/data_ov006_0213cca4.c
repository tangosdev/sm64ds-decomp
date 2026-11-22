/* claimed-storage */
/* ov006 .data.gap56 0x0213cca4..0x0213cce0. 4 storage addresses, one object. */
#include "types.h"

extern u32 _ZTVN3abi20__si_class_type_infoE[];
extern u32 _ZTS12dScMgJump2_c[];
extern u32 _ZTI14dScMgD3DBase_c[];
extern u32 sMgTextSpritesEnglish[];
extern u32 sMgTextSpritesSpanish[];
extern u32 sMgTextSpritesFrench[];
extern u32 sMgTextSpritesGerman[];
extern u32 sMgTextSpritesItalian[];

#pragma define_section claimsec ".data.gap56" ".data.gap56" ".data.gap56" far_data
#pragma section claimsec begin
#pragma explicit_zero_data on
u32 _ZTI12dScMgJump2_c[] = {
    (u32)&_ZTVN3abi20__si_class_type_infoE,
    (u32)&_ZTS12dScMgJump2_c,
    (u32)&_ZTI14dScMgD3DBase_c,
    0x00003600, 0x00003600, 0x00001000, 0x53643231, 0x4a674d63, 0x32706d75, 0x0000635f,
    (u32)&sMgTextSpritesEnglish,
    (u32)&sMgTextSpritesSpanish,
    (u32)&sMgTextSpritesFrench,
    (u32)&sMgTextSpritesGerman,
    (u32)&sMgTextSpritesItalian,
};
#pragma explicit_zero_data off
#pragma section claimsec end
