/* dScBoot/dScStage RTTI, spawn profiles and the sublevel globals.
 * g_profile_BOOT and g_profile_STAGE (classInit + 0x0150), the _ZTS/_ZTI
 * records of dScBoot/dScStage and their graph callbacks, and the
 * gNextSublevel/gCheckpointSublevel/gReturnSublevel/gCurrentAreaId/
 * gLastCourseSublevel/gPlayerHealth save-and-progress words.
 * Consumers: d_s_boot, d_s_stage, daPly_c.
 * ROM 0x020914a8..0x020914e0, 56 bytes, 5 configured symbols.
 * A _ZTI record's first word is its abi RTTI vtable address
 * (__si_class_type_info 0x0209a764); it stays a number because the tree
 * declares no _ZTV array.
 *
 * Order: mwcc emits one .data section per object and the linker
 * concatenates them in declaration order, so the definitions below are
 * in the order that yields the ascending addresses above.
 */
#include "types.h"

extern char _ZTS8dScene_c[12];
extern char _ZTS9dScBoot_c[12];
extern u32 _ZTI8dScene_c[3];
extern struct dScBoot_c *_Z19dScBoot_c_classInitv();
extern u32 _ZTI7dBase_c[];

/* 0x020914c8: 12 bytes. */
u32 _ZTI9dScBoot_c[3] = {
    0x0209a764,
    (u32)_ZTS9dScBoot_c,
    (u32)_ZTI8dScene_c,
};

/* 0x020914bc: 12 bytes. */
char _ZTS9dScBoot_c[12] = "9dScBoot_c";

/* 0x020914b0: 12 bytes. */
char _ZTS8dScene_c[12] = "8dScene_c";

/* 0x020914a8: 8 bytes. */
u32 g_profile_BOOT[2] = {
    (u32)&_Z19dScBoot_c_classInitv,
    0x00020000,
};

/* 0x020914d4: 12 bytes. */
u32 _ZTI8dScene_c[3] = {
    0x0209a764,
    (u32)_ZTS8dScene_c,
    (u32)_ZTI7dBase_c,
};
