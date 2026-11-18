/* ov080 data_ov080_0212808c at 0x0212808c (data, 36 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov080_0212808c[9] = {
    0x0201013c,  /* _ZN8dActor_c8OnKickedERS_ */
    0x02010138,  /* _ZN8dActor_c8OnPushedERS_ */
    0x02010134,  /* _ZN8dActor_c24OnHitByCannonBlastedCharERS_ */
    0x02010130,  /* _ZN8dActor_c15OnHitByMegaCharER7daPly_c */
    0x0201012c,  /* _ZN8dActor_c19OnHitFromUnderneathERS_ */
    0x02010124,  /* _ZN8dActor_c16OnAimedAtWithEggEv */
    0x020100dc,  /* _ZN8dActor_c25OnAimedAtWithEggReturnVecEv */
    0x00000000,
    0x02127fbc,  /* _ZTI11daChoropu_c */
};

#pragma section dc8_section end
