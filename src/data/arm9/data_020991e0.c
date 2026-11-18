/* Collision and 3D-line RTTI, gArctanTable.
 * _ZTI/_ZTS/_ZTV of dBgCh_Actr, dBgPi/dBgPc, dM3dGLin and dM3dGSph.
 * ROM 0x020991e0..0x02099204, 36 bytes, 2 configured symbols.
 * A _ZTI record's first word is its abi RTTI vtable address
 * (__si_class_type_info 0x0209a764); it stays a number because the tree
 * declares no _ZTV array.
 */
#include "types.h"

extern u32 _ZTI10dBgCh_Actr[3];
extern u32 _ZTS10dBgCh_Actr[6];
extern u8 _ZTI5dBgCh[];

/* 0x020991e0: 12 bytes. */
u32 _ZTI10dBgCh_Actr[3] = {
    0x0209a764,
    (u32)_ZTS10dBgCh_Actr,
    (u32)_ZTI5dBgCh,
};

/* 0x020991ec: 24 bytes. */
u32 _ZTS10dBgCh_Actr[6] = {
    0x42643031, 0x5f684367, 0x72746341, 0x00000000, 0x00000000, (u32)_ZTI10dBgCh_Actr,
};
