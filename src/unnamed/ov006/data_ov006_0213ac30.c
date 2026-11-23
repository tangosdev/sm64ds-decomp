#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    void *f000;
} sSlotLamp1Sprite_Layout;
typedef char sSlotLamp1Sprite_size[sizeof(sSlotLamp1Sprite_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[2];
} data_ov006_0213ac34_Layout;
typedef char data_ov006_0213ac34_size[sizeof(data_ov006_0213ac34_Layout) == 0x8 ? 1 : -1];

extern sSlotLamp1Sprite_Layout sSlotLamp1Sprite;
extern data_ov006_0213ac34_Layout data_ov006_0213ac34;

#pragma define_section sName ".data.gap43"                                                         \
                             ".data.gap43"                                                         \
                             ".data.gap43" far_data
#pragma section sName begin

sSlotLamp1Sprite_Layout sSlotLamp1Sprite = {
    &data_ov006_0213ac34,
};

data_ov006_0213ac34_Layout data_ov006_0213ac34 = {{
    0xc1e000e0,
    0xffff5018,
}};
