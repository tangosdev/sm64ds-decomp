/* Crash-screen text and the dCc RTTI records.
 * The printf format strings ShowCrashScreen prints (RSP, R0-R12, PC, RA,
 * CPSR registers), then the _ZTS/_ZTI records of the dCc/dCcAc/dCcPos/
 * dCcAcPos/dClipper classes and the dExt* model family.
 * ROM 0x0208e9d8..0x0208ea50, 120 bytes, 10 configured symbols.
 * A _ZTI record's first word is its abi RTTI vtable address
 * (__si_class_type_info 0x0209a764, __class_type_info 0x0209a774); it stays
 * a number because the tree declares no _ZTV array.
 *
 * Order: mwcc emits one .data section per object and the linker
 * concatenates them in declaration order, so the definitions below are
 * in the order that yields the ascending addresses above.
 */
#include "types.h"

extern u32 _ZTI8dFader_c[2];
extern char _ZTS8dFader_c[12];
extern char _ZTS9dFdWipe_c[12];
extern u32 _ZTI10dFdColor_c[3];
extern u32 _ZTI15dFdBrightness_c[3];
extern char _ZTS10dFdDummy_c[16];
extern char _ZTS10dFdColor_c[16];
extern void _ZThn80_N15dExtAnm2Model_cD1Ev();
extern void _ZThn80_N15dExtAnm2Model_cD0Ev();
extern u8 _ZTS15dFdBrightness_c[];

/* 0x0208ea30: 16 bytes. */
char _ZTS10dFdDummy_c[16] = "10dFdDummy_c";

/* 0x0208ea24: 12 bytes. */
u32 _ZTI15dFdBrightness_c[3] = {
    0x0209a764,
    (u32)_ZTS15dFdBrightness_c,
    (u32)_ZTI8dFader_c,
};

/* 0x0208ea00: 12 bytes. */
u32 _ZTI10dFdColor_c[3] = {
    0x0209a764,
    (u32)_ZTS10dFdColor_c,
    (u32)_ZTI15dFdBrightness_c,
};

/* 0x0208ea0c: 12 bytes. */
u32 _ZTI10dFdDummy_c[3] = {
    0x0209a764,
    (u32)_ZTS10dFdDummy_c,
    (u32)_ZTI10dFdColor_c,
};

/* 0x0208e9e8: 12 bytes. */
char _ZTS8dFader_c[12] = "8dFader_c";

/* 0x0208e9f4: 12 bytes. */
char _ZTS9dFdWipe_c[12] = "9dFdWipe_c";

/* 0x0208ea18: 12 bytes. */
u32 _ZTI9dFdWipe_c[3] = {
    0x0209a764,
    (u32)_ZTS9dFdWipe_c,
    (u32)_ZTI10dFdColor_c,
};

/* 0x0208e9d8: 8 bytes. */
u32 VTable_dExtFrameCtrl_c_dExtAnm2Model_cThunk[2] = {
    (u32)&_ZThn80_N15dExtAnm2Model_cD1Ev,
    (u32)&_ZThn80_N15dExtAnm2Model_cD0Ev,
};

/* 0x0208e9e0: 8 bytes. */
u32 _ZTI8dFader_c[2] = {
    0x0209a774,
    (u32)_ZTS8dFader_c,
};

/* 0x0208ea40: 16 bytes. */
char _ZTS10dFdColor_c[16] = "10dFdColor_c";
