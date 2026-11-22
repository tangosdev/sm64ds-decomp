/* claimed-storage */
/* ov006 .data.gap57 0x0213ce54..0x0213cee0. 7 storage addresses, one object. */
#include "types.h"

extern u32 _ZTVN3abi20__si_class_type_infoE[];
extern u32 _ZTS12dScMgLuigi_c[];
extern u32 _ZTI11dScMgBase_c[];
extern u32 sMgTextSpritesEnglish[];
extern u32 sMgTextSpritesSpanish[];
extern u32 sMgTextSpritesFrench[];
extern u32 sMgTextSpritesGerman[];
extern u32 sMgTextSpritesItalian[];

#pragma define_section claimsec ".data.gap57" ".data.gap57" ".data.gap57" far_data
#pragma section claimsec begin
#pragma explicit_zero_data on
u32 _ZTI12dScMgLuigi_c[] = {
    (u32)&_ZTVN3abi20__si_class_type_infoE,
    (u32)&_ZTS12dScMgLuigi_c,
    (u32)&_ZTI11dScMgBase_c,
    0x53643231, 0x4c674d63, 0x69676975, 0x0000635f,
    (u32)&sMgTextSpritesEnglish,
    (u32)&sMgTextSpritesSpanish,
    (u32)&sMgTextSpritesFrench,
    (u32)&sMgTextSpritesGerman,
    (u32)&sMgTextSpritesItalian,
    0x01000000, 0x00000001, 0x00000000, 0x00000000, 0x00000000, 0x00030201, 0x00000300, 0x00000400,
    0x00000000, 0x00000000, 0x00000000, 0x00000300, 0x06020000, 0x02000700, 0x00060000, 0x02010300,
    0x02010300, 0x00020003, 0x00020103, 0x01020301, 0x01020103, 0x01020100, 0x00000001,
};
#pragma explicit_zero_data off
#pragma section claimsec end
