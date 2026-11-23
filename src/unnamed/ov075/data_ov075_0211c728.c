#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    void *f000;
    u32 f004;
} data_ov075_0211c728_Layout;
typedef char data_ov075_0211c728_size[sizeof(data_ov075_0211c728_Layout) == 0x8 ? 1 : -1];

extern data_ov075_0211c728_Layout data_ov075_0211c728;
extern u8 _ZN10dScEntry_c12EnterResultsEv[];

#pragma define_section sName ".data.entry_sprite_gap2"                                             \
                             ".data.entry_sprite_gap2"                                             \
                             ".data.entry_sprite_gap2" far_data
#pragma section sName begin

data_ov075_0211c728_Layout data_ov075_0211c728 = {
    &_ZN10dScEntry_c12EnterResultsEv[0],
    0x00000000,
};
