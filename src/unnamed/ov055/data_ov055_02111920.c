#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[1];
} data_ov055_02111920_Layout;
typedef char data_ov055_02111920_size[sizeof(data_ov055_02111920_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[1];
} data_ov055_02111924_Layout;
typedef char data_ov055_02111924_size[sizeof(data_ov055_02111924_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[1];
} data_ov055_02111928_Layout;
typedef char data_ov055_02111928_size[sizeof(data_ov055_02111928_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[1];
} data_ov055_0211192c_Layout;
typedef char data_ov055_0211192c_size[sizeof(data_ov055_0211192c_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 f000;
    void *f004;
} data_ov055_02111930_Layout;
typedef char data_ov055_02111930_size[sizeof(data_ov055_02111930_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 f000;
    void *f004;
} data_ov055_02111938_Layout;
typedef char data_ov055_02111938_size[sizeof(data_ov055_02111938_Layout) == 0x8 ? 1 : -1];

extern data_ov055_02111920_Layout data_ov055_02111920;
extern data_ov055_02111924_Layout data_ov055_02111924;
extern data_ov055_02111928_Layout data_ov055_02111928;
extern data_ov055_0211192c_Layout data_ov055_0211192c;
extern data_ov055_02111930_Layout data_ov055_02111930;
extern data_ov055_02111938_Layout data_ov055_02111938;
extern u8 data_ov055_02111958[];
extern u8 data_ov055_02111a00[];

#pragma define_section sName ".data.lgPre"                                                         \
                             ".data.lgPre"                                                         \
                             ".data.lgPre" far_data
#pragma section sName begin

data_ov055_02111920_Layout data_ov055_02111920 = {{
    0x0000071f,
}};

#pragma explicit_zero_data on
data_ov055_0211192c_Layout data_ov055_0211192c = {0};
#pragma explicit_zero_data off

data_ov055_02111928_Layout data_ov055_02111928 = {{
    0xff020000,
}};

data_ov055_02111924_Layout data_ov055_02111924 = {{
    0x00000a28,
}};

data_ov055_02111930_Layout data_ov055_02111930 = {
    0x00000002,
    &data_ov055_02111958[0],
};

data_ov055_02111938_Layout data_ov055_02111938 = {
    0x00000006,
    &data_ov055_02111a00[0],
};
