#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[2];
} data_ov006_0213a550_Layout;
typedef char data_ov006_0213a550_size[sizeof(data_ov006_0213a550_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[4];
} data_ov006_0213a558_Layout;
typedef char data_ov006_0213a558_size[sizeof(data_ov006_0213a558_Layout) == 0x10 ? 1 : -1];

typedef struct {
    void *f000;
    void *f004;
    void *f008;
    void *f00c;
} sAmidaLaneWantSprites_Layout;
typedef char sAmidaLaneWantSprites_size[sizeof(sAmidaLaneWantSprites_Layout) == 0x10 ? 1 : -1];

typedef struct {
    u32 w[8];
} data_ov006_0213a578_Layout;
typedef char data_ov006_0213a578_size[sizeof(data_ov006_0213a578_Layout) == 0x20 ? 1 : -1];

typedef struct {
    u32 w[8];
} data_ov006_0213a598_Layout;
typedef char data_ov006_0213a598_size[sizeof(data_ov006_0213a598_Layout) == 0x20 ? 1 : -1];

extern data_ov006_0213a550_Layout data_ov006_0213a550;
extern data_ov006_0213a558_Layout data_ov006_0213a558;
extern sAmidaLaneWantSprites_Layout sAmidaLaneWantSprites;
extern data_ov006_0213a578_Layout data_ov006_0213a578;
extern data_ov006_0213a598_Layout data_ov006_0213a598;

#pragma define_section sName ".data.gap38"                                                         \
                             ".data.gap38"                                                         \
                             ".data.gap38" far_data
#pragma section sName begin

data_ov006_0213a550_Layout data_ov006_0213a550 = {{
    0x81f000f0,
    0xffff2026,
}};

sAmidaLaneWantSprites_Layout sAmidaLaneWantSprites = {
    &data_ov006_0213a598,
    &data_ov006_0213a550,
    &data_ov006_0213a578,
    &data_ov006_0213a558,
};

data_ov006_0213a558_Layout data_ov006_0213a558 = {{
    0x81f000e8,
    0x00004013,
    0x41f04008,
    0xffff4093,
}};

data_ov006_0213a578_Layout data_ov006_0213a578 = {{
    0x81e800e8,
    0x0000300b,
    0x800880e8,
    0x0000300f,
    0x41e84008,
    0x0000308b,
    0x00084008,
    0xffff308f,
}};

data_ov006_0213a598_Layout data_ov006_0213a598 = {{
    0x81e800e8,
    0x00001000,
    0x800880e8,
    0x00001004,
    0x41e84008,
    0x00001080,
    0x00084008,
    0xffff1084,
}};
