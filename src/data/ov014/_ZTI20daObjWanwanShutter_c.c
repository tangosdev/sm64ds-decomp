/* ov014 cartridge data at 0x02114868, 12 bytes. */
#include "types.h"
#include "rtti_abi.h"

extern u32 _ZTI10dBgActor_c[];
extern char _ZTS20daObjWanwanShutter_c[];

#pragma define_section _ZTI20daObjWanwanShutter_c ".data.gap3"                                     \
                                                  ".data.gap3"                                     \
                                                  ".data.gap3" far_data
#pragma section _ZTI20daObjWanwanShutter_c begin
u32 _ZTI20daObjWanwanShutter_c[3] = {
    (u32)&_ZTVN3abi20__si_class_type_infoE,
    (u32)&_ZTS20daObjWanwanShutter_c,
    (u32)&_ZTI10dBgActor_c,
};
#pragma section _ZTI20daObjWanwanShutter_c end
