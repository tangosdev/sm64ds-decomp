#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    void *f000;
    u32 f004;
} data_ov075_0211c878_Layout;
typedef char data_ov075_0211c878_size[sizeof(data_ov075_0211c878_Layout) == 0x8 ? 1 : -1];

extern data_ov075_0211c878_Layout data_ov075_0211c878;
extern u8 _ZN10dScEntry_c15EnterPlayerSyncEv[];

#pragma define_section sName ".data.entry_sprite_gap6"                                             \
                             ".data.entry_sprite_gap6"                                             \
                             ".data.entry_sprite_gap6" far_data
#pragma section sName begin

data_ov075_0211c878_Layout data_ov075_0211c878 = {
    &_ZN10dScEntry_c15EnterPlayerSyncEv[0],
    0x00000000,
};
