#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    void *f000;
    u32 f004;
} data_ov075_0211c780_Layout;
typedef char data_ov075_0211c780_size[sizeof(data_ov075_0211c780_Layout) == 0x8 ? 1 : -1];

extern data_ov075_0211c780_Layout data_ov075_0211c780;
extern u8 _ZN10dScEntry_c17RenderWaitingTextEv[];

#pragma define_section sName ".data.entry_sprite_gap4"                                             \
                             ".data.entry_sprite_gap4"                                             \
                             ".data.entry_sprite_gap4" far_data
#pragma section sName begin

data_ov075_0211c780_Layout data_ov075_0211c780 = {
    &_ZN10dScEntry_c17RenderWaitingTextEv[0],
    0x00000000,
};
