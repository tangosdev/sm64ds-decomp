/* Collision and 3D-line RTTI, gArctanTable.
 * _ZTI/_ZTS/_ZTV of dBgCh_Actr, dBgPi/dBgPc, dM3dGLin and dM3dGSph.
 * ROM 0x020991e0..0x02099204, 36 bytes, 2 configured symbols.
 */
#include "types.h"

extern u32 _ZTI10dBgCh_Actr[3];
extern u32 _ZTS10dBgCh_Actr[6];
extern u8 _ZTVN3abi20__si_class_type_infoE[];
extern u8 _ZTI5dBgCh[];

/* 0x020991e0: 12 bytes. */
u32 _ZTI10dBgCh_Actr[3] = {
    (u32)_ZTVN3abi20__si_class_type_infoE,
    (u32)_ZTS10dBgCh_Actr,
    (u32)_ZTI5dBgCh,
};

/* 0x020991ec: 24 bytes. */
u32 _ZTS10dBgCh_Actr[6] = {
    0x42643031, 0x5f684367, 0x72746341, 0x00000000, 0x00000000, (u32)_ZTI10dBgCh_Actr,
};
