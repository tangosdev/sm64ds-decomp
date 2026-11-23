#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    void *f000;
    u32 f004;
} data_ov075_0211c898_Layout;
typedef char data_ov075_0211c898_size[sizeof(data_ov075_0211c898_Layout) == 0x8 ? 1 : -1];

typedef struct {
    void *f000;
    u32 f004;
} data_ov075_0211c8a0_Layout;
typedef char data_ov075_0211c8a0_size[sizeof(data_ov075_0211c8a0_Layout) == 0x8 ? 1 : -1];

typedef struct {
    void *f000;
    u32 f004;
} data_ov075_0211c8a8_Layout;
typedef char data_ov075_0211c8a8_size[sizeof(data_ov075_0211c8a8_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[2];
} data_ov075_0211c8b0_Layout;
typedef char data_ov075_0211c8b0_size[sizeof(data_ov075_0211c8b0_Layout) == 0x8 ? 1 : -1];

typedef struct {
    void *f000;
    u32 f004;
} data_ov075_0211c8b8_Layout;
typedef char data_ov075_0211c8b8_size[sizeof(data_ov075_0211c8b8_Layout) == 0x8 ? 1 : -1];

typedef struct {
    void *f000;
    u32 f004;
} data_ov075_0211c8c0_Layout;
typedef char data_ov075_0211c8c0_size[sizeof(data_ov075_0211c8c0_Layout) == 0x8 ? 1 : -1];

typedef struct {
    void *f000;
    u32 f004;
} data_ov075_0211c8c8_Layout;
typedef char data_ov075_0211c8c8_size[sizeof(data_ov075_0211c8c8_Layout) == 0x8 ? 1 : -1];

typedef struct {
    void *f000;
    u32 f004;
} data_ov075_0211c8d0_Layout;
typedef char data_ov075_0211c8d0_size[sizeof(data_ov075_0211c8d0_Layout) == 0x8 ? 1 : -1];

typedef struct {
    void *f000;
    u32 f004;
} data_ov075_0211c8d8_Layout;
typedef char data_ov075_0211c8d8_size[sizeof(data_ov075_0211c8d8_Layout) == 0x8 ? 1 : -1];

extern data_ov075_0211c898_Layout data_ov075_0211c898;
extern data_ov075_0211c8a0_Layout data_ov075_0211c8a0;
extern data_ov075_0211c8a8_Layout data_ov075_0211c8a8;
extern data_ov075_0211c8b0_Layout data_ov075_0211c8b0;
extern data_ov075_0211c8b8_Layout data_ov075_0211c8b8;
extern data_ov075_0211c8c0_Layout data_ov075_0211c8c0;
extern data_ov075_0211c8c8_Layout data_ov075_0211c8c8;
extern data_ov075_0211c8d0_Layout data_ov075_0211c8d0;
extern data_ov075_0211c8d8_Layout data_ov075_0211c8d8;
extern u8 _ZN10dScEntry_c18UpdateCourseSelectEv[];
extern u8 _ZN10dScEntry_c16EnterQuitMessageEv[];
extern u8 _ZN10dScEntry_c17EnterMatchLoadingEv[];
extern u8 _ZN10dScEntry_c15UpdateSoloStartEv[];
extern u8 _ZN10dScEntry_c17EnterSessionStartEv[];
extern u8 _ZN10dScEntry_c17RenderRecruitTextEv[];
extern u8 _ZN10dScEntry_c15UpdateStepDelayEv[];
extern u8 _ZN10dScEntry_c18UpdateSessionStartEv[];

#pragma define_section sName ".data.entry_sprite_gap7"                                             \
                             ".data.entry_sprite_gap7"                                             \
                             ".data.entry_sprite_gap7" far_data
#pragma section sName begin

data_ov075_0211c8d0_Layout data_ov075_0211c8d0 = {
    &_ZN10dScEntry_c15UpdateStepDelayEv[0],
    0x00000000,
};

data_ov075_0211c8c8_Layout data_ov075_0211c8c8 = {
    &_ZN10dScEntry_c17RenderRecruitTextEv[0],
    0x00000000,
};

data_ov075_0211c8c0_Layout data_ov075_0211c8c0 = {
    &_ZN10dScEntry_c17EnterSessionStartEv[0],
    0x00000000,
};

data_ov075_0211c8b8_Layout data_ov075_0211c8b8 = {
    &_ZN10dScEntry_c15UpdateSoloStartEv[0],
    0x00000000,
};

data_ov075_0211c8b0_Layout data_ov075_0211c8b0 = {{
    0x01f480f8,
    0xffff14c0,
}};

data_ov075_0211c8a8_Layout data_ov075_0211c8a8 = {
    &_ZN10dScEntry_c17EnterMatchLoadingEv[0],
    0x00000000,
};

data_ov075_0211c8a0_Layout data_ov075_0211c8a0 = {
    &_ZN10dScEntry_c16EnterQuitMessageEv[0],
    0x00000000,
};

data_ov075_0211c898_Layout data_ov075_0211c898 = {
    &_ZN10dScEntry_c18UpdateCourseSelectEv[0],
    0x00000000,
};

data_ov075_0211c8d8_Layout data_ov075_0211c8d8 = {
    &_ZN10dScEntry_c18UpdateSessionStartEv[0],
    0x00000000,
};
