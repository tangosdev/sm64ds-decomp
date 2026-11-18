/* Collision and 3D-line RTTI, gArctanTable.
 * _ZTI/_ZTS/_ZTV of dBgCh_Actr, dBgPi/dBgPc, dM3dGLin and dM3dGSph.
 * ROM 0x0209920c..0x02099224, 24 bytes, 3 configured symbols.
 * A _ZTI record's first word is its abi RTTI vtable address
 * (__class_type_info 0x0209a774); it stays a number because the tree
 * declares no _ZTV array.
 *
 * Order: mwcc emits one .data section per object and the linker
 * concatenates them in declaration order, so the definitions below are
 * in the order that yields the ascending addresses above.
 */
#include "types.h"

extern char _ZTS5dBgPc[8];

/* 0x02099214: 8 bytes. */
char _ZTS5dBgPc[8] = "5dBgPc";

/* 0x0209920c: 8 bytes. */
char _ZTS5dBgPi[8] = "5dBgPi";

/* 0x0209921c: 8 bytes. */
u32 _ZTI5dBgPc[2] = {
    0x0209a774,
    (u32)_ZTS5dBgPc,
};
