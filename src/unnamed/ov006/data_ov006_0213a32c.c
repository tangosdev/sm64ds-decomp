#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    void *f000;
} sAmidaWalkerSprite_Layout;
typedef char sAmidaWalkerSprite_size[sizeof(sAmidaWalkerSprite_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[2];
} data_ov006_0213a330_Layout;
typedef char data_ov006_0213a330_size[sizeof(data_ov006_0213a330_Layout) == 0x8 ? 1 : -1];

extern sAmidaWalkerSprite_Layout sAmidaWalkerSprite;
extern data_ov006_0213a330_Layout data_ov006_0213a330;

#pragma define_section sName ".data.gap38"                                                         \
                             ".data.gap38"                                                         \
                             ".data.gap38" far_data
#pragma section sName begin

sAmidaWalkerSprite_Layout sAmidaWalkerSprite = {
    &data_ov006_0213a330,
};

data_ov006_0213a330_Layout data_ov006_0213a330 = {{
    0x81f000f0,
    0xffff4013,
}};
