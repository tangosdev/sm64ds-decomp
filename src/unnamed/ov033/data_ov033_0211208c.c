#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[6];
} data_ov033_0211208c_Layout;
typedef char data_ov033_0211208c_size[sizeof(data_ov033_0211208c_Layout) == 0x18 ? 1 : -1];

typedef struct {
    u32 w[6];
} data_ov033_021120a4_Layout;
typedef char data_ov033_021120a4_size[sizeof(data_ov033_021120a4_Layout) == 0x18 ? 1 : -1];

typedef struct {
    u32 w[7];
} data_ov033_021120bc_Layout;
typedef char data_ov033_021120bc_size[sizeof(data_ov033_021120bc_Layout) == 0x1c ? 1 : -1];

typedef struct {
    u32 w[8];
} data_ov033_021120d8_Layout;
typedef char data_ov033_021120d8_size[sizeof(data_ov033_021120d8_Layout) == 0x20 ? 1 : -1];

extern data_ov033_0211208c_Layout data_ov033_0211208c;
extern data_ov033_021120a4_Layout data_ov033_021120a4;
extern data_ov033_021120bc_Layout data_ov033_021120bc;
extern data_ov033_021120d8_Layout data_ov033_021120d8;

#pragma define_section sName ".data.gap1"                                                          \
                             ".data.gap1"                                                          \
                             ".data.gap1" far_data
#pragma section sName begin

data_ov033_0211208c_Layout data_ov033_0211208c = {{
    0xfd440019,
    0xf95cfc4a,
    0x00000000,
    0xff000000,
    0xfe0c0019,
    0xf95cfc4a,
}};

data_ov033_021120a4_Layout data_ov033_021120a4 = {{
    0x00000000,
    0xff000000,
    0xfe0c0019,
    0xf894fc4a,
    0x00000000,
    0xff000000,
}};

data_ov033_021120bc_Layout data_ov033_021120bc = {{
    0xfed40019,
    0xf894fc4a,
    0x00000000,
    0xff000000,
    0xfe0c0019,
    0xf7ccfc4a,
    0x00000000,
}};

data_ov033_021120d8_Layout data_ov033_021120d8 = {{
    0xff000000,
    0xfed40019,
    0xf7ccfc4a,
    0x00000000,
    0xff000000,
    0xff9c0019,
    0xf7ccfc4a,
    0x00000000,
}};
