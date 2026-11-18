/* dScBoot/dScStage RTTI, spawn profiles and the sublevel globals.
 * g_profile_BOOT and g_profile_STAGE (classInit + 0x0150), the _ZTS/_ZTI
 * records of dScBoot/dScStage and their graph callbacks, and the
 * gNextSublevel/gCheckpointSublevel/gReturnSublevel/gCurrentAreaId/
 * gLastCourseSublevel/gPlayerHealth save-and-progress words.
 * Consumers: d_s_boot, d_s_stage, daPly_c.
 * ROM 0x02092128..0x02092188, 96 bytes, 11 configured symbols.
 * A _ZTI record's first word is its abi RTTI vtable address
 * (__si_class_type_info 0x0209a764); it stays a number because the tree
 * declares no _ZTV array.
 *
 * Order: mwcc emits one .data section per object and the linker
 * concatenates them in declaration order, so the definitions below are
 * in the order that yields the ascending addresses above.
 */
#include "types.h"

extern u32 _ZTIN10dScStage_c15graphCallback_cE[3];
extern u32 _ZTS10dScStage_c[6];
extern struct dScStage_c *_Z20dScStage_c_classInitv();
extern u8 data_02075650[];
extern u8 data_02075670[];
extern u8 data_02075690[];
extern u32 _ZTI8dScene_c[];
extern u32 _ZTSN10dScStage_c15graphCallback_cE[];
extern u32 _ZTIN8dGraph_c10callback_cE[];

/* 0x02092164: 12 bytes. */
u32 _ZTIN10dScStage_c15graphCallback_cE[3] = {
    0x0209a764,
    (u32)_ZTSN10dScStage_c15graphCallback_cE,
    (u32)_ZTIN8dGraph_c10callback_cE,
};

/* 0x0209214c: 12 bytes. */
unsigned short *data_0209214c[3] = {
    (unsigned short *)&data_02075650,
    (unsigned short *)&data_02075670,
    (unsigned short *)&data_02075690,
};

/* 0x02092158: 12 bytes. */
u32 _ZTI10dScStage_c[3] = {
    0x0209a764,
    (u32)_ZTS10dScStage_c,
    (u32)_ZTI8dScene_c,
};

/* 0x02092144: 8 bytes. */
short gPlayerHealth[4] = {
    0x0880,
    0x0880,
    0x0880,
    0x0880,
};

/* 0x0209213c: 8 bytes. */
u32 g_profile_STAGE[2] = {
    (u32)&_Z20dScStage_c_classInitv,
    0x00060003,
};

/* 0x02092138: 4 bytes. */
int data_02092138 = (int)0x80000000u;

/* 0x02092134: 4 bytes. */
unsigned int data_02092134 = 0x44444444u;

/* 0x02092130: 4 bytes. */
int data_02092130 = (int)0xffffffffu;

/* 0x0209212c: 4 bytes. */
int data_0209212c = 0x7fffffff;

/* 0x02092128: 4 bytes. */
u8 data_02092128[4] = {
    0x00,
    0x01,
    0x02,
    0x03,
};

/* 0x02092170: 24 bytes. */
u32 _ZTS10dScStage_c[6] = {
    0x53643031, 0x61745363, 0x635f6567,
    0x00000000, 0x00000000, (u32)_ZTIN10dScStage_c15graphCallback_cE,
};
