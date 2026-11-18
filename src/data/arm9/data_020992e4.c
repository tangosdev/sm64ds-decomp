/* Collision and 3D-line RTTI, gArctanTable.
 * _ZTI/_ZTS/_ZTV of dBgCh_Actr, dBgPi/dBgPc, dM3dGLin and dM3dGSph.
 * ROM 0x020992e4..0x020992f8, 20 bytes, 2 configured symbols.
 * A _ZTI record's first word is its abi RTTI vtable address
 * (__class_type_info 0x0209a774); it stays a number because the tree
 * declares no _ZTV array.
 */
#include "types.h"

extern char _ZTS8dM3dGSph[12];

/* 0x020992e4: 8 bytes. */
u32 _ZTI8dM3dGSph[2] = {
    0x0209a774,
    (u32)_ZTS8dM3dGSph,
};

/* 0x020992ec: 12 bytes. */
char _ZTS8dM3dGSph[12] = "8dM3dGSph";
