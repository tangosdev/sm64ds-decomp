#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[2];
} data_ov075_0211c710_Layout;
typedef char data_ov075_0211c710_size[sizeof(data_ov075_0211c710_Layout) == 0x8 ? 1 : -1];

typedef struct {
    void *f000;
    u32 f004;
} data_ov075_0211c718_Layout;
typedef char data_ov075_0211c718_size[sizeof(data_ov075_0211c718_Layout) == 0x8 ? 1 : -1];

extern data_ov075_0211c710_Layout data_ov075_0211c710;
extern data_ov075_0211c718_Layout data_ov075_0211c718;
extern u8 _ZN10dScEntry_c17RenderWaitingTextEv[];

#pragma define_section sName ".data.entry_sprite_gap1"                                             \
                             ".data.entry_sprite_gap1"                                             \
                             ".data.entry_sprite_gap1" far_data
#pragma section sName begin

data_ov075_0211c710_Layout data_ov075_0211c710 = {{
    0x001c001f,
    0xffffe11f,
}};

data_ov075_0211c718_Layout data_ov075_0211c718 = {
    &_ZN10dScEntry_c17RenderWaitingTextEv[0],
    0x00000000,
};
