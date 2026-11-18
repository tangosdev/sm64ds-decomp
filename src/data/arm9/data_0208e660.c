/* Crash-screen text and the dCc RTTI records.
 * The printf format strings ShowCrashScreen prints (RSP, R0-R12, PC, RA,
 * CPSR registers), then the _ZTS/_ZTI records of the dCc/dCcAc/dCcPos/
 * dCcAcPos/dClipper classes and the dExt* model family.
 * ROM 0x0208e660..0x0208e6bc, 92 bytes, 7 configured symbols.
 *
 * Order: mwcc emits one .data section per object and the linker
 * concatenates them in declaration order, so the definitions below are
 * in the order that yields the ascending addresses above.
 */
#include "types.h"

extern u32 _ZTI5dCc_c[2];
extern char _ZTS7dCcAc_c[12];
extern char _ZTS8dCcPos_c[12];
extern u32 _ZTI8dCcPos_c[3];
extern u32 _ZTI7dCcAc_c[3];
extern u32 _ZTS10dCcAcPos_c[6];
extern u8 _ZTVN3abi17__class_type_infoE[];
extern char _ZTS5dCc_c[];
extern u8 _ZTVN3abi20__si_class_type_infoE[];

/* 0x0208e698: 12 bytes. */
u32 _ZTI7dCcAc_c[3] = {
    (u32)_ZTVN3abi20__si_class_type_infoE,
    (u32)_ZTS7dCcAc_c,
    (u32)_ZTI5dCc_c,
};

/* 0x0208e68c: 12 bytes. */
u32 _ZTI8dCcPos_c[3] = {
    (u32)_ZTVN3abi20__si_class_type_infoE,
    (u32)_ZTS8dCcPos_c,
    (u32)_ZTI5dCc_c,
};

/* 0x0208e680: 12 bytes. */
u32 _ZTI10dCcAcPos_c[3] = {
    (u32)_ZTVN3abi20__si_class_type_infoE,
    (u32)_ZTS10dCcAcPos_c,
    (u32)_ZTI7dCcAc_c,
};

/* 0x0208e674: 12 bytes. */
char _ZTS8dCcPos_c[12] = "8dCcPos_c";

/* 0x0208e668: 12 bytes. */
char _ZTS7dCcAc_c[12] = "7dCcAc_c";

/* 0x0208e660: 8 bytes. */
u32 _ZTI5dCc_c[2] = {
    (u32)_ZTVN3abi17__class_type_infoE,
    (u32)_ZTS5dCc_c,
};

/* 0x0208e6a4: 24 bytes. */
u32 _ZTS10dCcAcPos_c[6] = {
    0x43643031, 0x50634163, 0x635f736f, 0x00000000, 0x00000000, (u32)_ZTI8dCcPos_c,
};
