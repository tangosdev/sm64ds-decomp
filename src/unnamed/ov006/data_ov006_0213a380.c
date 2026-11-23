#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[4];
} data_ov006_0213a380_Layout;
typedef char data_ov006_0213a380_size[sizeof(data_ov006_0213a380_Layout) == 0x10 ? 1 : -1];

typedef struct {
    void *f000;
    void *f004;
    void *f008;
    void *f00c;
} sAmidaBoardAnimSprites_Layout;
typedef char sAmidaBoardAnimSprites_size[sizeof(sAmidaBoardAnimSprites_Layout) == 0x10 ? 1 : -1];

typedef struct {
    u32 w[6];
} data_ov006_0213a3a0_Layout;
typedef char data_ov006_0213a3a0_size[sizeof(data_ov006_0213a3a0_Layout) == 0x18 ? 1 : -1];

typedef struct {
    u32 w[6];
} data_ov006_0213a3b8_Layout;
typedef char data_ov006_0213a3b8_size[sizeof(data_ov006_0213a3b8_Layout) == 0x18 ? 1 : -1];

typedef struct {
    u32 w[6];
} data_ov006_0213a3d0_Layout;
typedef char data_ov006_0213a3d0_size[sizeof(data_ov006_0213a3d0_Layout) == 0x18 ? 1 : -1];

extern data_ov006_0213a380_Layout data_ov006_0213a380;
extern sAmidaBoardAnimSprites_Layout sAmidaBoardAnimSprites;
extern data_ov006_0213a3a0_Layout data_ov006_0213a3a0;
extern data_ov006_0213a3b8_Layout data_ov006_0213a3b8;
extern data_ov006_0213a3d0_Layout data_ov006_0213a3d0;

#pragma define_section sName ".data.gap38"                                                         \
                             ".data.gap38"                                                         \
                             ".data.gap38" far_data
#pragma section sName begin

data_ov006_0213a380_Layout data_ov006_0213a380 = {{
    0x81ef00e0,
    0x00003000,
    0x81f04000,
    0xffff205a,
}};

sAmidaBoardAnimSprites_Layout sAmidaBoardAnimSprites = {
    &data_ov006_0213a380,
    &data_ov006_0213a3b8,
    &data_ov006_0213a3a0,
    &data_ov006_0213a3d0,
};

data_ov006_0213a3a0_Layout data_ov006_0213a3a0 = {{
    0x81eb00e0,
    0x00003009,
    0x400b80e0,
    0x0000300d,
    0x81f04000,
    0xffff205a,
}};

data_ov006_0213a3b8_Layout data_ov006_0213a3b8 = {{
    0x81eb00e0,
    0x00003004,
    0x400b80e0,
    0x00003008,
    0x81f04000,
    0xffff205a,
}};

data_ov006_0213a3d0_Layout data_ov006_0213a3d0 = {{
    0x81ec00e0,
    0x0000300e,
    0x400c80e0,
    0x00003012,
    0x81f04000,
    0xffff205a,
}};
