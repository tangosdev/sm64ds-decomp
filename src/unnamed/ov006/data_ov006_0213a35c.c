#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    void *f000;
} sAmidaSideSprite_Layout;
typedef char sAmidaSideSprite_size[sizeof(sAmidaSideSprite_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[3];
} data_ov006_0213a360_Layout;
typedef char data_ov006_0213a360_size[sizeof(data_ov006_0213a360_Layout) == 0xc ? 1 : -1];

typedef struct {
    u32 w[5];
} data_ov006_0213a36c_Layout;
typedef char data_ov006_0213a36c_size[sizeof(data_ov006_0213a36c_Layout) == 0x14 ? 1 : -1];

extern sAmidaSideSprite_Layout sAmidaSideSprite;
extern data_ov006_0213a360_Layout data_ov006_0213a360;
extern data_ov006_0213a36c_Layout data_ov006_0213a36c;

#pragma define_section sName ".data.gap38"                                                         \
                             ".data.gap38"                                                         \
                             ".data.gap38" far_data
#pragma section sName begin

sAmidaSideSprite_Layout sAmidaSideSprite = {
    &data_ov006_0213a360,
};

data_ov006_0213a360_Layout data_ov006_0213a360 = {{
    0x81f480ec,
    0x00000017,
    0x400480ec,
}};

data_ov006_0213a36c_Layout data_ov006_0213a36c = {{
    0x00000019,
    0x01f4400c,
    0x0000001a,
    0x0004000c,
    0xffff001c,
}};
