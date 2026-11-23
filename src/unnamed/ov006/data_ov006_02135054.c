#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    void *f000;
    void *f004;
} sPanelHudSprites_Layout;
typedef char sPanelHudSprites_size[sizeof(sPanelHudSprites_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[8];
} data_ov006_0213505c_Layout;
typedef char data_ov006_0213505c_size[sizeof(data_ov006_0213505c_Layout) == 0x20 ? 1 : -1];

typedef struct {
    u32 w[8];
} data_ov006_0213507c_Layout;
typedef char data_ov006_0213507c_size[sizeof(data_ov006_0213507c_Layout) == 0x20 ? 1 : -1];

extern sPanelHudSprites_Layout sPanelHudSprites;
extern data_ov006_0213505c_Layout data_ov006_0213505c;
extern data_ov006_0213507c_Layout data_ov006_0213507c;

#pragma define_section sName ".data.gap18"                                                         \
                             ".data.gap18"                                                         \
                             ".data.gap18" far_data
#pragma section sName begin

sPanelHudSprites_Layout sPanelHudSprites = {
    &data_ov006_0213505c,
    &data_ov006_0213507c,
};

data_ov006_0213505c_Layout data_ov006_0213505c = {{
    0x81c040f8,
    0x00000080,
    0x81e040f8,
    0x00000084,
    0x800040f8,
    0x00000088,
    0x802040f8,
    0xffff008c,
}};

data_ov006_0213507c_Layout data_ov006_0213507c = {{
    0x81c040f8,
    0x000000c0,
    0x81e040f8,
    0x000000c4,
    0x800040f8,
    0x000000c8,
    0x802040f8,
    0xffff00cc,
}};
