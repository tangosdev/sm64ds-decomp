#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    void *f000;
} sSlotLamp2Sprite_Layout;
typedef char sSlotLamp2Sprite_size[sizeof(sSlotLamp2Sprite_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[2];
} data_ov006_0213ac40_Layout;
typedef char data_ov006_0213ac40_size[sizeof(data_ov006_0213ac40_Layout) == 0x8 ? 1 : -1];

extern sSlotLamp2Sprite_Layout sSlotLamp2Sprite;
extern data_ov006_0213ac40_Layout data_ov006_0213ac40;

#pragma define_section sName ".data.gap43"                                                         \
                             ".data.gap43"                                                         \
                             ".data.gap43" far_data
#pragma section sName begin

sSlotLamp2Sprite_Layout sSlotLamp2Sprite = {
    &data_ov006_0213ac40,
};

data_ov006_0213ac40_Layout data_ov006_0213ac40 = {{
    0x81f000f0,
    0xffff5074,
}};
