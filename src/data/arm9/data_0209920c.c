/* Collision and 3D-line RTTI, gArctanTable.
 * _ZTI/_ZTS/_ZTV of dBgCh_Actr, dBgPi/dBgPc, dM3dGLin and dM3dGSph.
 * ROM 0x0209920c..0x02099224, 24 bytes, 3 configured symbols.
 *
 * Order: mwcc emits one .data section per object and the linker
 * concatenates them in declaration order, so the definitions below are
 * in the order that yields the ascending addresses above.
 */
#include "types.h"

extern char _ZTS5dBgPc[8];
extern u8 _ZTVN3abi17__class_type_infoE[];

/* 0x02099214: 8 bytes. */
char _ZTS5dBgPc[8] = "5dBgPc";

/* 0x0209920c: 8 bytes. */
char _ZTS5dBgPi[8] = "5dBgPi";

/* 0x0209921c: 8 bytes. */
u32 _ZTI5dBgPc[2] = {
    (u32)_ZTVN3abi17__class_type_infoE,
    (u32)_ZTS5dBgPc,
};
