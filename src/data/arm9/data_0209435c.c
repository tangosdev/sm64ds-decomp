/* Multiboot scene profile and RTTI.
 * g_profile_MULTIBOOT, the _ZTS/_ZTI records of dScMB_c and its graph
 * callback, and the 64-byte scene record that follows.
 * ROM 0x0209435c..0x02094390, 52 bytes, 4 configured symbols.
 *
 * Order: mwcc emits one .data section per object and the linker
 * concatenates them in declaration order, so the definitions below are
 * in the order that yields the ascending addresses above.
 */
#include "types.h"

extern char _ZTS7dScMB_c[12];
extern u32 _ZTIN7dScMB_c15graphCallback_cE[5];
extern void *_Z17dScMB_c_classInitv();
extern u8 _ZTVN3abi20__si_class_type_infoE[];
extern u32 _ZTI8dScene_c[];
extern u32 _ZTSN7dScMB_c15graphCallback_cE[];
extern u32 _ZTIN8dGraph_c10callback_cE[];

/* 0x02094370: 12 bytes. */
u32 _ZTI7dScMB_c[3] = {
    (u32)_ZTVN3abi20__si_class_type_infoE,
    (u32)_ZTS7dScMB_c,
    (u32)_ZTI8dScene_c,
};

/* 0x02094364: 12 bytes. */
char _ZTS7dScMB_c[12] = "7dScMB_c";

/* 0x0209435c: 8 bytes. */
u32 g_profile_MULTIBOOT[2] = {
    (u32)&_Z17dScMB_c_classInitv,
    0x01680168,
};

/* 0x0209437c: 20 bytes. */
u32 _ZTIN7dScMB_c15graphCallback_cE[5] = {
    (u32)_ZTVN3abi20__si_class_type_infoE, (u32)_ZTSN7dScMB_c15graphCallback_cE,
    (u32)_ZTIN8dGraph_c10callback_cE,      0x00000000,
    (u32)_ZTIN7dScMB_c15graphCallback_cE,
};
