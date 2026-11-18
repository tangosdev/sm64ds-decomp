/* ov014 cartridge data at 0x02114718, 12 bytes. */
#include "types.h"

extern u32 _ZTI12dEnemyBase_c[];
extern u32 _ZTS10daWanwan_c[];
extern u32 _ZTVN3abi20__si_class_type_infoE[];

#pragma define_section _ZTI10daWanwan_c ".data.gap1" ".data.gap1" ".data.gap1" far_data
#pragma section _ZTI10daWanwan_c begin
u32 _ZTI10daWanwan_c[3] = {
    (u32)&_ZTVN3abi20__si_class_type_infoE, (u32)&_ZTS10daWanwan_c, (u32)&_ZTI12dEnemyBase_c,
};
#pragma section _ZTI10daWanwan_c end
