#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[2];
} data_ov006_02136d38_Layout;
typedef char data_ov006_02136d38_size[sizeof(data_ov006_02136d38_Layout) == 0x8 ? 1 : -1];

typedef struct {
    void *f000;
    void *f004;
    void *f008;
    void *f00c;
} sPanelCursorSprites_Layout;
typedef char sPanelCursorSprites_size[sizeof(sPanelCursorSprites_Layout) == 0x10 ? 1 : -1];

typedef struct {
    u32 w[4];
} data_ov006_02136d50_Layout;
typedef char data_ov006_02136d50_size[sizeof(data_ov006_02136d50_Layout) == 0x10 ? 1 : -1];

typedef struct {
    u32 w[6];
} data_ov006_02136d60_Layout;
typedef char data_ov006_02136d60_size[sizeof(data_ov006_02136d60_Layout) == 0x18 ? 1 : -1];

typedef struct {
    u32 w[8];
} data_ov006_02136d78_Layout;
typedef char data_ov006_02136d78_size[sizeof(data_ov006_02136d78_Layout) == 0x20 ? 1 : -1];

extern data_ov006_02136d38_Layout data_ov006_02136d38;
extern sPanelCursorSprites_Layout sPanelCursorSprites;
extern data_ov006_02136d50_Layout data_ov006_02136d50;
extern data_ov006_02136d60_Layout data_ov006_02136d60;
extern data_ov006_02136d78_Layout data_ov006_02136d78;

#pragma define_section sName ".data.gap22"                                                         \
                             ".data.gap22"                                                         \
                             ".data.gap22" far_data
#pragma section sName begin

data_ov006_02136d38_Layout data_ov006_02136d38 = {{
    0xd1f080e0,
    0xffff001c,
}};

data_ov006_02136d50_Layout data_ov006_02136d50 = {{
    0x41f900e1,
    0x00000016,
    0xd1f080e0,
    0xffff001c,
}};

sPanelCursorSprites_Layout sPanelCursorSprites = {
    &data_ov006_02136d38,
    &data_ov006_02136d50,
    &data_ov006_02136d60,
    &data_ov006_02136d78,
};

data_ov006_02136d60_Layout data_ov006_02136d60 = {{
    0x41f900f0,
    0x00000016,
    0x41f900e1,
    0x00000016,
    0xd1f080e0,
    0xffff001c,
}};

data_ov006_02136d78_Layout data_ov006_02136d78 = {{
    0x41f900ff,
    0x00000016,
    0x41f900f0,
    0x00000016,
    0x41f900e1,
    0x00000016,
    0xd1f080e0,
    0xffff001c,
}};
