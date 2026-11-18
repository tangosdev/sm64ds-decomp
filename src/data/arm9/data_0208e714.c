/* Crash-screen text and the dCc RTTI records.
 * The printf format strings ShowCrashScreen prints (RSP, R0-R12, PC, RA,
 * CPSR registers), then the _ZTS/_ZTI records of the dCc/dCcAc/dCcPos/
 * dCcAcPos/dClipper classes and the dExt* model family.
 * ROM 0x0208e714..0x0208e730, 28 bytes, 2 configured symbols.
 * A _ZTI record's first word is its abi RTTI vtable address
 * (__class_type_info 0x0209a774); it stays a number because the tree
 * declares no _ZTV array.
 */
#include "types.h"

extern u32 _ZTI8dClipper[2];
extern u32 _ZTS8dClipper[5];

/* 0x0208e714: 8 bytes. */
u32 _ZTI8dClipper[2] = {
    0x0209a774,
    (u32)_ZTS8dClipper,
};

/* 0x0208e71c: 20 bytes. */
u32 _ZTS8dClipper[5] = {
    0x6c436438, 0x65707069, 0x00000072, 0x00000000, (u32)_ZTI8dClipper,
};
