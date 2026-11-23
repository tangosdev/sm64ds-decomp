#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    void *f000;
} sAmidaHurryButtonSprite_Layout;
typedef char sAmidaHurryButtonSprite_size[sizeof(sAmidaHurryButtonSprite_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[8];
} data_ov006_0213a33c_Layout;
typedef char data_ov006_0213a33c_size[sizeof(data_ov006_0213a33c_Layout) == 0x20 ? 1 : -1];

extern sAmidaHurryButtonSprite_Layout sAmidaHurryButtonSprite;
extern data_ov006_0213a33c_Layout data_ov006_0213a33c;

#pragma define_section sName ".data.gap38"                                                         \
                             ".data.gap38"                                                         \
                             ".data.gap38" far_data
#pragma section sName begin

sAmidaHurryButtonSprite_Layout sAmidaHurryButtonSprite = {
    &data_ov006_0213a33c,
};

data_ov006_0213a33c_Layout data_ov006_0213a33c = {{
    0x81ec40f8,
    0x0000009b,
    0x000c80f8,
    0x0000009f,
    0x81e000f0,
    0x0000001b,
    0x800000f0,
    0xffff001c,
}};
