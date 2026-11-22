/* claimed-storage */
/* ov006 .data.gap58 0x0213d338..0x0213d40c. 7 storage addresses, one object. */
#include "types.h"

extern u32 data_ov006_0213d384[];
extern u32 data_ov006_0213d3c4[];
extern u32 data_ov006_0213d40c[];
extern u32 _ZTVN3abi20__si_class_type_infoE[];
extern u32 _ZTS14dScMgMemory2_c[];
extern u32 _ZTI19dScMgSingle3DBase_c[];

#pragma define_section claimsec ".data.gap58" ".data.gap58" ".data.gap58" far_data
#pragma section claimsec begin
#pragma explicit_zero_data on
u32 sMemory2CardPositionsPerDifficulty[] = {
    (u32)&data_ov006_0213d384,
    (u32)&data_ov006_0213d3c4,
    (u32)&data_ov006_0213d40c,
    0x00060008, 0x00060004, 0x00180018,
    (u32)&_ZTVN3abi20__si_class_type_infoE,
    (u32)&_ZTS14dScMgMemory2_c,
    (u32)&_ZTI19dScMgSingle3DBase_c,
    0x53643431, 0x4d674d63, 0x726f6d65, 0x635f3279, 0x00000000, 0x000000ba, 0x000000bc, 0x000000b7,
    0x000000b8, 0x000000b9, 0x00a80050, 0x00a80070, 0x00a80090, 0x00a800b0, 0x00780050, 0x00780070,
    0x00780090, 0x007800b0, 0x00480050, 0x00480070, 0x00480090, 0x004800b0, 0x00180050, 0x00180070,
    0x00180090, 0x001800b0, 0x00900030, 0x00900050, 0x00900070, 0x00900090, 0x009000b0, 0x009000d0,
    0x00600030, 0x00600050, 0x00600070, 0x00600090, 0x006000b0, 0x006000d0, 0x00300030, 0x00300050,
    0x00300070, 0x00300090, 0x003000b0, 0x003000d0,
};
#pragma explicit_zero_data off
#pragma section claimsec end
