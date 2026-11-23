#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[2];
} data_ov033_02111b2c_Layout;
typedef char data_ov033_02111b2c_size[sizeof(data_ov033_02111b2c_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[2];
} data_ov033_02111b34_Layout;
typedef char data_ov033_02111b34_size[sizeof(data_ov033_02111b34_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[2];
} data_ov033_02111b3c_Layout;
typedef char data_ov033_02111b3c_size[sizeof(data_ov033_02111b3c_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[2];
} data_ov033_02111b44_Layout;
typedef char data_ov033_02111b44_size[sizeof(data_ov033_02111b44_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[4];
} data_ov033_02111b4c_Layout;
typedef char data_ov033_02111b4c_size[sizeof(data_ov033_02111b4c_Layout) == 0x10 ? 1 : -1];

typedef struct {
    u32 w[4];
} data_ov033_02111b5c_Layout;
typedef char data_ov033_02111b5c_size[sizeof(data_ov033_02111b5c_Layout) == 0x10 ? 1 : -1];

extern data_ov033_02111b2c_Layout data_ov033_02111b2c;
extern data_ov033_02111b34_Layout data_ov033_02111b34;
extern data_ov033_02111b3c_Layout data_ov033_02111b3c;
extern data_ov033_02111b44_Layout data_ov033_02111b44;
extern data_ov033_02111b4c_Layout data_ov033_02111b4c;
extern data_ov033_02111b5c_Layout data_ov033_02111b5c;

#pragma define_section sName ".data.gap0"                                                          \
                             ".data.gap0"                                                          \
                             ".data.gap0" far_data
#pragma section sName begin

data_ov033_02111b2c_Layout data_ov033_02111b2c = {{
    0x012c00fc,
    0x076cfe70,
}};

data_ov033_02111b44_Layout data_ov033_02111b44 = {{
    0x00000000,
    0x41010000,
}};

data_ov033_02111b3c_Layout data_ov033_02111b3c = {{
    0xfc7300fc,
    0xff7d0384,
}};

data_ov033_02111b34_Layout data_ov033_02111b34 = {{
    0x00000000,
    0xf0020000,
}};

data_ov033_02111b4c_Layout data_ov033_02111b4c = {{
    0x012c00fc,
    0x076cfe70,
    0x00000000,
    0xf0020000,
}};

data_ov033_02111b5c_Layout data_ov033_02111b5c = {{
    0x012c00fc,
    0x076cfe70,
    0x00000000,
    0xf0020000,
}};
