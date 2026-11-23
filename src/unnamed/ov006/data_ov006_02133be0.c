#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[4];
} data_ov006_02133be0_Layout;
typedef char data_ov006_02133be0_size[sizeof(data_ov006_02133be0_Layout) == 0x10 ? 1 : -1];

typedef struct {
    u32 w[4];
} data_ov006_02133bf0_Layout;
typedef char data_ov006_02133bf0_size[sizeof(data_ov006_02133bf0_Layout) == 0x10 ? 1 : -1];

typedef struct {
    u32 w[4];
} data_ov006_02133c00_Layout;
typedef char data_ov006_02133c00_size[sizeof(data_ov006_02133c00_Layout) == 0x10 ? 1 : -1];

typedef struct {
    u32 w[6];
} data_ov006_02133c10_Layout;
typedef char data_ov006_02133c10_size[sizeof(data_ov006_02133c10_Layout) == 0x18 ? 1 : -1];

typedef struct {
    u32 w[6];
} data_ov006_02133c28_Layout;
typedef char data_ov006_02133c28_size[sizeof(data_ov006_02133c28_Layout) == 0x18 ? 1 : -1];

typedef struct {
    u32 w[6];
} data_ov006_02133c40_Layout;
typedef char data_ov006_02133c40_size[sizeof(data_ov006_02133c40_Layout) == 0x18 ? 1 : -1];

typedef struct {
    u32 w[6];
} data_ov006_02133c58_Layout;
typedef char data_ov006_02133c58_size[sizeof(data_ov006_02133c58_Layout) == 0x18 ? 1 : -1];

extern data_ov006_02133be0_Layout data_ov006_02133be0;
extern data_ov006_02133bf0_Layout data_ov006_02133bf0;
extern data_ov006_02133c00_Layout data_ov006_02133c00;
extern data_ov006_02133c10_Layout data_ov006_02133c10;
extern data_ov006_02133c28_Layout data_ov006_02133c28;
extern data_ov006_02133c40_Layout data_ov006_02133c40;
extern data_ov006_02133c58_Layout data_ov006_02133c58;

#pragma define_section sName ".data.gap4"                                                          \
                             ".data.gap4"                                                          \
                             ".data.gap4" far_data
#pragma section sName begin

data_ov006_02133c00_Layout data_ov006_02133c00 = {{
    0xc1e84000,
    0x00003017,
    0xc1e840e0,
    0xffff408f,
}};

data_ov006_02133be0_Layout data_ov006_02133be0 = {{
    0xc1e84000,
    0x00003017,
    0x800000e0,
    0xffff4012,
}};

data_ov006_02133bf0_Layout data_ov006_02133bf0 = {{
    0xc1e04000,
    0x00003016,
    0xc1e040e0,
    0xffff4097,
}};

data_ov006_02133c28_Layout data_ov006_02133c28 = {{
    0xc1e04000,
    0x00003016,
    0x81e800e0,
    0x00004101,
    0x800880e0,
    0xffff4105,
}};

data_ov006_02133c10_Layout data_ov006_02133c10 = {{
    0xc1e04000,
    0x00003016,
    0x81e800e0,
    0x00004081,
    0x800880e0,
    0xffff4085,
}};

data_ov006_02133c40_Layout data_ov006_02133c40 = {{
    0x81e040f0,
    0x00004040,
    0x800040f0,
    0x00004044,
    0xc1e04000,
    0xffff3016,
}};

data_ov006_02133c58_Layout data_ov006_02133c58 = {{
    0x81e040f0,
    0x00004000,
    0x800040f0,
    0x00004004,
    0xc1e04000,
    0xffff3016,
}};
