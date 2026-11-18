/* Crash-screen and debug font.
 * data_0208c178 is a 512-byte 1bpp glyph strip; data_0208c378 is the 8192-byte
 * 8x8 ASCII font the OS_Terminate screen and LoadDebugFont draw from (the
 * first 36 bytes are the terminated 'A' cell and the rest is the blank tail).
 * Consumers: InitCrashScreen, LoadDebugFont, d_s_title, d_actor.
 * ROM 0x0208e37c..0x0208e3a4, 40 bytes, 4 configured symbols.
 * A _ZTI record's first word is its abi RTTI vtable address
 * (__si_class_type_info 0x0209a764); it stays a number because the tree
 * declares no _ZTV array.
 *
 * Order: mwcc emits one .data section per object and the linker
 * concatenates them in declaration order, so the definitions below are
 * in the order that yields the ascending addresses above.
 */
#include "types.h"

extern char _ZTS8dActor_c[12];
extern u32 _ZTI8dActor_c[5];
extern u32 _ZTI7dBase_c[];

/* 0x0208e384: 12 bytes. */
char _ZTS8dActor_c[12] = "8dActor_c";

/* 0x0208e380: 4 bytes. */
int data_0208e380 = 0x7fffffff;

/* 0x0208e37c: 4 bytes. */
int data_0208e37c = (int)0x80000000u;

/* 0x0208e390: 20 bytes. */
u32 _ZTI8dActor_c[5] = {
    0x0209a764, (u32)_ZTS8dActor_c, (u32)_ZTI7dBase_c, 0x00000000, (u32)_ZTI8dActor_c,
};
