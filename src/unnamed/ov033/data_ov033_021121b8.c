#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[2];
} data_ov033_021121b8_Layout;
typedef char data_ov033_021121b8_size[sizeof(data_ov033_021121b8_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[7];
} data_ov033_021121c0_Layout;
typedef char data_ov033_021121c0_size[sizeof(data_ov033_021121c0_Layout) == 0x1c ? 1 : -1];

typedef struct {
    u32 w[7];
} data_ov033_021121dc_Layout;
typedef char data_ov033_021121dc_size[sizeof(data_ov033_021121dc_Layout) == 0x1c ? 1 : -1];

typedef struct {
    u32 w[24];
} data_ov033_021121f8_Layout;
typedef char data_ov033_021121f8_size[sizeof(data_ov033_021121f8_Layout) == 0x60 ? 1 : -1];

extern data_ov033_021121b8_Layout data_ov033_021121b8;
extern data_ov033_021121c0_Layout data_ov033_021121c0;
extern data_ov033_021121dc_Layout data_ov033_021121dc;
extern data_ov033_021121f8_Layout data_ov033_021121f8;

#pragma define_section sName ".data.gap1"                                                          \
                             ".data.gap1"                                                          \
                             ".data.gap1" far_data
#pragma section sName begin

data_ov033_021121b8_Layout data_ov033_021121b8 = {{
    0xff000000,
    0x00640019,
}};

data_ov033_021121c0_Layout data_ov033_021121c0 = {{
    0xf3e4fc4a,
    0x00000000,
    0xff000000,
    0x00640019,
    0xf31cfc4a,
    0x00000000,
    0xff000000,
}};

data_ov033_021121dc_Layout data_ov033_021121dc = {{
    0x00640019,
    0xf254fc4a,
    0x00000000,
    0xff000000,
    0x00000019,
    0xf18cfc4a,
    0x00000000,
}};

data_ov033_021121f8_Layout data_ov033_021121f8 = {{
    0xff000000, 0x00000019, 0xf0c4fc4a, 0x00000000, 0xff000000, 0xfed40019, 0xf704fc4a, 0x00000000,
    0xff000000, 0xff9c00a1, 0x0384fdab, 0x00000000, 0xffff0000, 0xf8f80010, 0x06a4fc9a, 0x00000000,
    0xffff0000, 0xfb500010, 0x02bcff6a, 0x00000000, 0xffff0000, 0xfc7c0010, 0xfce00100, 0x00000000,
}};
