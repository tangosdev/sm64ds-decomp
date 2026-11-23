#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    void *f000;
} sRouletteRacerSprites_Layout;
typedef char sRouletteRacerSprites_size[sizeof(sRouletteRacerSprites_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[2];
} data_ov006_02138c40_Layout;
typedef char data_ov006_02138c40_size[sizeof(data_ov006_02138c40_Layout) == 0x8 ? 1 : -1];

extern sRouletteRacerSprites_Layout sRouletteRacerSprites;
extern data_ov006_02138c40_Layout data_ov006_02138c40;

#pragma define_section sName ".data.gap32"                                                         \
                             ".data.gap32"                                                         \
                             ".data.gap32" far_data
#pragma section sName begin

sRouletteRacerSprites_Layout sRouletteRacerSprites = {
    &data_ov006_02138c40,
};

data_ov006_02138c40_Layout data_ov006_02138c40 = {{
    0x41f800f8,
    0xffff0008,
}};
