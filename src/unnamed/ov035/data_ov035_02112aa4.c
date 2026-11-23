#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    void *f000;
} g_profile_CT_MECHA10_Layout;
typedef char g_profile_CT_MECHA10_size[sizeof(g_profile_CT_MECHA10_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[6];
} data_ov035_02112aa8_Layout;
typedef char data_ov035_02112aa8_size[sizeof(data_ov035_02112aa8_Layout) == 0x18 ? 1 : -1];

typedef struct {
    void *f000;
    u32 f004;
    u32 f008;
    u32 f00c;
    u32 f010;
    u32 f014;
    u32 f018;
} g_profile_CT_MECHA12L_Layout;
typedef char g_profile_CT_MECHA12L_size[sizeof(g_profile_CT_MECHA12L_Layout) == 0x1c ? 1 : -1];

typedef struct {
    void *f000;
    u32 f004;
    u32 f008;
    u32 f00c;
    u32 f010;
    u32 f014;
    u32 f018;
    u32 f01c;
    void *f020;
} g_profile_CT_MECHA12S_Layout;
typedef char g_profile_CT_MECHA12S_size[sizeof(g_profile_CT_MECHA12S_Layout) == 0x24 ? 1 : -1];

extern g_profile_CT_MECHA10_Layout g_profile_CT_MECHA10;
extern data_ov035_02112aa8_Layout data_ov035_02112aa8;
extern g_profile_CT_MECHA12L_Layout g_profile_CT_MECHA12L;
extern g_profile_CT_MECHA12S_Layout g_profile_CT_MECHA12S;
extern u8 _Z37daObjCtMecha10_c_classInit_CT_MECHA10v[];
extern u8 _Z38daObjCtMecha10_c_classInit_CT_MECHA12Lv[];
extern u8 _Z38daObjCtMecha10_c_classInit_CT_MECHA12Sv[];
extern u8 _ZTI16daObjCtMecha10_c[];

#pragma define_section sName ".data.m35Mid2"                                                       \
                             ".data.m35Mid2"                                                       \
                             ".data.m35Mid2" far_data
#pragma section sName begin

g_profile_CT_MECHA10_Layout g_profile_CT_MECHA10 = {
    &_Z37daObjCtMecha10_c_classInit_CT_MECHA10v[0],
};

data_ov035_02112aa8_Layout data_ov035_02112aa8 = {{
    0x00f20077,
    0x00000002,
    0x00060000,
    0x01000000,
    0x01000000,
    0x00000000,
}};

g_profile_CT_MECHA12L_Layout g_profile_CT_MECHA12L = {
    &_Z38daObjCtMecha10_c_classInit_CT_MECHA12Lv[0],
    0x00f40079,
    0x00000003,
    0x00060000,
    0x00250000,
    0x01000000,
    0x00000000,
};

g_profile_CT_MECHA12S_Layout g_profile_CT_MECHA12S = {
    &_Z38daObjCtMecha10_c_classInit_CT_MECHA12Sv[0],
    0x00f5007a,
    0x00000003,
    0x00060000,
    0x00250000,
    0x01000000,
    0x00000000,
    0x00000000,
    &_ZTI16daObjCtMecha10_c[0],
};
