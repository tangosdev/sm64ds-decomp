#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[2];
} data_ov075_0211c790_Layout;
typedef char data_ov075_0211c790_size[sizeof(data_ov075_0211c790_Layout) == 0x8 ? 1 : -1];

typedef struct {
    void *f000;
    u32 f004;
} data_ov075_0211c798_Layout;
typedef char data_ov075_0211c798_size[sizeof(data_ov075_0211c798_Layout) == 0x8 ? 1 : -1];

typedef struct {
    void *f000;
    u32 f004;
} data_ov075_0211c7a0_Layout;
typedef char data_ov075_0211c7a0_size[sizeof(data_ov075_0211c7a0_Layout) == 0x8 ? 1 : -1];

typedef struct {
    void *f000;
    u32 f004;
} data_ov075_0211c7a8_Layout;
typedef char data_ov075_0211c7a8_size[sizeof(data_ov075_0211c7a8_Layout) == 0x8 ? 1 : -1];

typedef struct {
    void *f000;
    u32 f004;
} data_ov075_0211c7b0_Layout;
typedef char data_ov075_0211c7b0_size[sizeof(data_ov075_0211c7b0_Layout) == 0x8 ? 1 : -1];

typedef struct {
    void *f000;
    u32 f004;
} data_ov075_0211c7b8_Layout;
typedef char data_ov075_0211c7b8_size[sizeof(data_ov075_0211c7b8_Layout) == 0x8 ? 1 : -1];

extern data_ov075_0211c790_Layout data_ov075_0211c790;
extern data_ov075_0211c798_Layout data_ov075_0211c798;
extern data_ov075_0211c7a0_Layout data_ov075_0211c7a0;
extern data_ov075_0211c7a8_Layout data_ov075_0211c7a8;
extern data_ov075_0211c7b0_Layout data_ov075_0211c7b0;
extern data_ov075_0211c7b8_Layout data_ov075_0211c7b8;
extern u8 _ZN10dScEntry_c15RenderWinCountsEv[];
extern u8 _ZN10dScEntry_c21RenderControlSettingsEv[];
extern u8 _ZN10dScEntry_c16EnterQuitConfirmEv[];
extern u8 _ZN10dScEntry_c14EnterSoloStartEv[];
extern u8 _ZN10dScEntry_c19func_ov075_02118a78Ev[];

#pragma define_section sName ".data.entry_sprite_gap4"                                             \
                             ".data.entry_sprite_gap4"                                             \
                             ".data.entry_sprite_gap4" far_data
#pragma section sName begin

data_ov075_0211c7b0_Layout data_ov075_0211c7b0 = {
    &_ZN10dScEntry_c14EnterSoloStartEv[0],
    0x00000000,
};

data_ov075_0211c7a8_Layout data_ov075_0211c7a8 = {
    &_ZN10dScEntry_c16EnterQuitConfirmEv[0],
    0x00000000,
};

data_ov075_0211c7a0_Layout data_ov075_0211c7a0 = {
    &_ZN10dScEntry_c21RenderControlSettingsEv[0],
    0x00000000,
};

data_ov075_0211c798_Layout data_ov075_0211c798 = {
    &_ZN10dScEntry_c15RenderWinCountsEv[0],
    0x00000000,
};

data_ov075_0211c790_Layout data_ov075_0211c790 = {{
    0x41f800f8,
    0xfffff5d2,
}};

data_ov075_0211c7b8_Layout data_ov075_0211c7b8 = {
    &_ZN10dScEntry_c19func_ov075_02118a78Ev[0],
    0x00000000,
};
