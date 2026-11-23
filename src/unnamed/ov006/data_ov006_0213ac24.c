#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    void *f000;
} sPachinkoDecoSprite_Layout;
typedef char sPachinkoDecoSprite_size[sizeof(sPachinkoDecoSprite_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[2];
} data_ov006_0213ac28_Layout;
typedef char data_ov006_0213ac28_size[sizeof(data_ov006_0213ac28_Layout) == 0x8 ? 1 : -1];

extern sPachinkoDecoSprite_Layout sPachinkoDecoSprite;
extern data_ov006_0213ac28_Layout data_ov006_0213ac28;

#pragma define_section sName ".data.gap43"                                                         \
                             ".data.gap43"                                                         \
                             ".data.gap43" far_data
#pragma section sName begin

sPachinkoDecoSprite_Layout sPachinkoDecoSprite = {
    &data_ov006_0213ac28,
};

data_ov006_0213ac28_Layout data_ov006_0213ac28 = {{
    0xc1e88000,
    0xffff211c,
}};
