#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[1];
} data_ov006_02134fe8_Layout;
typedef char data_ov006_02134fe8_size[sizeof(data_ov006_02134fe8_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[3];
} data_ov006_02134fec_Layout;
typedef char data_ov006_02134fec_size[sizeof(data_ov006_02134fec_Layout) == 0xc ? 1 : -1];

typedef struct {
    u32 w[4];
} data_ov006_02134ff8_Layout;
typedef char data_ov006_02134ff8_size[sizeof(data_ov006_02134ff8_Layout) == 0x10 ? 1 : -1];

typedef struct {
    u32 w[4];
} data_ov006_02135008_Layout;
typedef char data_ov006_02135008_size[sizeof(data_ov006_02135008_Layout) == 0x10 ? 1 : -1];

extern data_ov006_02134fe8_Layout data_ov006_02134fe8;
extern data_ov006_02134fec_Layout data_ov006_02134fec;
extern data_ov006_02134ff8_Layout data_ov006_02134ff8;
extern data_ov006_02135008_Layout data_ov006_02135008;

#pragma define_section sName ".data.gap18"                                                         \
                             ".data.gap18"                                                         \
                             ".data.gap18" far_data
#pragma section sName begin

data_ov006_02134fe8_Layout data_ov006_02134fe8 = {{
    0x41f800f7,
}};

data_ov006_02134fec_Layout data_ov006_02134fec = {{
    0x000011d2,
    0x81f000f0,
    0xffff2102,
}};

data_ov006_02134ff8_Layout data_ov006_02134ff8 = {{
    0x41f800f7,
    0x000011d2,
    0x81f000f0,
    0xffff0102,
}};

data_ov006_02135008_Layout data_ov006_02135008 = {{
    0x41f800f7,
    0x000011d2,
    0x81f000f0,
    0xffff1102,
}};
