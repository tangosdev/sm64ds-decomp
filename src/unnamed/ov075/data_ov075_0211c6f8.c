#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    void *f000;
    u32 f004;
} data_ov075_0211c6f8_Layout;
typedef char data_ov075_0211c6f8_size[sizeof(data_ov075_0211c6f8_Layout) == 0x8 ? 1 : -1];

typedef struct {
    void *f000;
    u32 f004;
} data_ov075_0211c700_Layout;
typedef char data_ov075_0211c700_size[sizeof(data_ov075_0211c700_Layout) == 0x8 ? 1 : -1];

extern data_ov075_0211c6f8_Layout data_ov075_0211c6f8;
extern data_ov075_0211c700_Layout data_ov075_0211c700;
extern u8 _ZN10dScEntry_c16EnterSoloConfirmEv[];
extern u8 _ZN10dScEntry_c16EnterStepFadeOutEv[];

#pragma define_section sName ".data.entry_sprite_gap0"                                             \
                             ".data.entry_sprite_gap0"                                             \
                             ".data.entry_sprite_gap0" far_data
#pragma section sName begin

data_ov075_0211c6f8_Layout data_ov075_0211c6f8 = {
    &_ZN10dScEntry_c16EnterSoloConfirmEv[0],
    0x00000000,
};

data_ov075_0211c700_Layout data_ov075_0211c700 = {
    &_ZN10dScEntry_c16EnterStepFadeOutEv[0],
    0x00000000,
};
