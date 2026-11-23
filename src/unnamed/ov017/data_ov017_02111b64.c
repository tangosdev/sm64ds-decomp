#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[1];
} data_ov017_02111b64_Layout;
typedef char data_ov017_02111b64_size[sizeof(data_ov017_02111b64_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[1];
} data_ov017_02111b68_Layout;
typedef char data_ov017_02111b68_size[sizeof(data_ov017_02111b68_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[2];
} data_ov017_02111b6c_Layout;
typedef char data_ov017_02111b6c_size[sizeof(data_ov017_02111b6c_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[5];
} data_ov017_02111b74_Layout;
typedef char data_ov017_02111b74_size[sizeof(data_ov017_02111b74_Layout) == 0x14 ? 1 : -1];

typedef struct {
    u32 w[11];
} data_ov017_02111b88_Layout;
typedef char data_ov017_02111b88_size[sizeof(data_ov017_02111b88_Layout) == 0x2c ? 1 : -1];

extern data_ov017_02111b64_Layout data_ov017_02111b64;
extern data_ov017_02111b68_Layout data_ov017_02111b68;
extern data_ov017_02111b6c_Layout data_ov017_02111b6c;
extern data_ov017_02111b74_Layout data_ov017_02111b74;
extern data_ov017_02111b88_Layout data_ov017_02111b88;

#pragma define_section sName ".data.ksRest"                                                        \
                             ".data.ksRest"                                                        \
                             ".data.ksRest" far_data
#pragma section sName begin

data_ov017_02111b64_Layout data_ov017_02111b64 = {{
    0x80000000,
}};

data_ov017_02111b68_Layout data_ov017_02111b68 = {{
    0x00800000,
}};

data_ov017_02111b6c_Layout data_ov017_02111b6c = {{
    0x0000423f,
    0x0b5405c6,
}};

data_ov017_02111b74_Layout data_ov017_02111b74 = {{
    0x012ce225,
    0xf574feac,
    0xff06e225,
    0x02bcffba,
    0x0226e225,
}};

data_ov017_02111b88_Layout data_ov017_02111b88 = {{
    0xf5d8feac,
    0x02eee225,
    0x0a5a039d,
    0x0320e225,
    0xf63cfeac,
    0x0352e225,
    0x03b6ffdd,
    0x00fae225,
    0x060e013b,
    0xff06e225,
    0x076c02a8,
}};
