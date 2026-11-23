#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    void *f000;
} data_ov055_02111940_Layout;
typedef char data_ov055_02111940_size[sizeof(data_ov055_02111940_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[2];
} data_ov055_02111944_Layout;
typedef char data_ov055_02111944_size[sizeof(data_ov055_02111944_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[3];
} data_ov055_0211194c_Layout;
typedef char data_ov055_0211194c_size[sizeof(data_ov055_0211194c_Layout) == 0xc ? 1 : -1];

extern data_ov055_02111940_Layout data_ov055_02111940;
extern data_ov055_02111944_Layout data_ov055_02111944;
extern data_ov055_0211194c_Layout data_ov055_0211194c;
extern u8 data_ov055_02111930[];

#pragma define_section sName ".data.lgPre"                                                         \
                             ".data.lgPre"                                                         \
                             ".data.lgPre" far_data
#pragma section sName begin

data_ov055_02111940_Layout data_ov055_02111940 = {
    &data_ov055_02111930[0],
};

data_ov055_02111944_Layout data_ov055_02111944 = {{
    0x00000000,
    0x07080300,
}};

data_ov055_0211194c_Layout data_ov055_0211194c = {{
    0x012cf704,
    0xf704fc81,
    0x037f012c,
}};
